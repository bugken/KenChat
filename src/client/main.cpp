#include "json.hpp"
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "group.hpp"
#include "user.hpp"
#include "public.hpp"

using namespace std;
using json=nlohmann::json;

//记录当前系统登录的用户信息
User g_currentUser;
//记录当前登录用户的好友列表信息
vector<User> g_currentUserFriendList;
//记录当前登录用户的群组列表信息
vector<Group> g_currentUserGroupList;
//显示当前登录成功用户的基本信息
void showCurrentUserData();
//接收线程
void readTaskHandler(int clientfd);
//获取系统时间(聊天信息需要添加时间信息)
string getCurrentTime();
//主聊天页面程序
void mainMenu();

//聊天客户端程序实现，main线程用作发送线程，子线程用作接收线程
int main(int argc, char** argv)
{
    if (argc < 3)
    {
        cerr << "command invalid. example:./ChatClient 127.0.0.1 6000" << endl;
        return -1;
    }
    //解析命令行参数传递的ip和port
    char* ip = argv[1];
    uint16_t port = atoi(argv[2]);

    //创建client的socket
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientfd)
    {
        cerr << "socket create error." << endl;
        return -1;
    }
    //填写client需要连接的server信息ip+port
    sockaddr_in server;
    memset(&server, 0 , sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);
    //client和server进行连接
    if (-1 == connect(clientfd, (sockaddr*)&server, sizeof(sockaddr_in)))
    {
        cerr << "connect server error." << endl;
        close(clientfd);
        return -1;
    }

    //main线程用于接收用户输入，负责发送数据
    for(;;)
    {
        //显示页面菜单 登录 注册 退出
        cout << "===============================" << endl;
        cout << "1. login" << endl;
        cout << "2. register" << endl;
        cout << "3. quit" << endl;
        cout << "===============================" << endl;
        cout << "choice:";
        int choice = 0;
        cin >> choice;
        cin.get();//读取缓冲区残留的回车
        switch (choice)
        {
        case 1://login业务
        {
            int id = 0;
            char pwd[50] = {0};
            cout << "userid:";
            cin >> id;
            cin.get();//读取缓冲区残留的回车
            cout << "userpassword:";
            cin.getline(pwd, 50);

            json js;
            js["msgid"] = LOGIN_MSG;
            js["id"] = id;
            js["password"] = pwd;
            string request = js.dump();

            int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
            if (-1 == len)
            {
                cerr << "send login msg error." << endl;
            }
            else
            {
                char buffer[1024] = {0};
                len = recv(clientfd, buffer, 1024, 0);
                if (-1 == len)
                {
                    cerr << "recv login response error." << endl;
                }
                else
                {
                    json response = json::parse(buffer);
                    if (0 != response["errno"].get<int>())//登录失败
                    {
                        cerr << response["errmsg"] << endl;
                    }
                    else//登录成功
                    {
                        //登录当前用户的id和name
                        g_currentUser.setId(response["id"].get<int>());
                        g_currentUser.setName(response["name"]);

                        //记录当前用户的好友列表信息
                        if (response.contains("friends"))
                        {
                            User user;
                            json js;
                            vector<string> vecFriends = response["friends"];
                            for (string &str : vecFriends)
                            {
                                js = json::parse(str);
                                user.setId(js["id"].get<int>());
                                user.setName(js["name"]);
                                user.setState(js["state"]);
                                g_currentUserFriendList.push_back(user);
                            }
                        }

                        //记录当前用户的群组列表信息
                        if (response.contains("groups"))
                        {
                            json jsgroups, jsgroupusers;
                            Group group;
                            GroupUser groupUser;
                            vector<string> vecGroups = response["groups"];
                            for (string &str : vecGroups)
                            {
                                jsgroups = json::parse(str);
                                group.setId(jsgroups["id"].get<int>());
                                group.setName(jsgroups["groupname"]);
                                group.setDesc(jsgroups["groupdesc"]);

                                vector<string> vecGroupUsers = jsgroups["users"];
                                for (string &userstr : vecGroupUsers)
                                {
                                    json jsgroupusers = json::parse(userstr);
                                    groupUser.setId(jsgroupusers["id"].get<int>());
                                    groupUser.setName(jsgroupusers["name"]);
                                    groupUser.setName(jsgroupusers["state"]);
                                    groupUser.setName(jsgroupusers["fole"]);
                                    group.getVecUsers().push_back(groupUser);
                                }
            
                                g_currentUserGroupList.push_back(group);
                            }
                        }
                        
                        //显示用户的基本信息
                        showCurrentUserData();

                        //显示当前用户的离线信息，个人聊天信息或者群组消息
                        if (response.contains("offlinemessage"))
                        {
                            json jsOfflineMessage;
                            vector<string> vecOfflinemessage = response["offlinemessage"];
                            for (string &offlinemsg : vecOfflinemessage)
                            {
                                jsOfflineMessage = json::parse(offlinemsg);
                                cout << jsOfflineMessage["time"] << " [" << jsOfflineMessage["id"] << "]" << js["name"] 
                                <<" said:" << jsOfflineMessage["msg"] << endl;
                            }
                        }

                        //登录成功后，启动接收线程负责接收数据
                        std::thread readTask(readTaskHandler, clientfd);
                        readTask.detach();

                        //进入聊天主页面
                        mainMenu();
                    }
                }
            }
        }
        break;
        case 2://注册业务
        {
            char name[50] = {0};
            char pwd[50] = {0};
            cout << "username:";
            cin.getline(name, 50);
            cout << "user password:";
            cin.getline(pwd, 50);

            json js;
            js["msgid"] = REG_MSG;
            js["name"] = name;
            js["password"] = pwd;
            string request = js.dump();

            int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
            if (len == -1)
            {
                cerr << "send reg msg error:" << request << endl;
            }
            else
            {
                char buffer[1024] = {0};
                len = recv(clientfd, buffer, 1024, 0);
                if(len == -1)
                {
                    cerr << "recv reg response error." << endl;
                }
                else
                {
                    json response = json::parse(buffer);
                    if (0 != response["errno"].get<int>())//注册失败
                    {
                        cerr << name << " is already exist, register error." << endl;
                    }//注册成功
                    else
                    {
                        cout << name << " register sucess, userid is " 
                            << response["id"] << ", do not forget it." << endl;
                    }
                }  
            }
        }
        break;
        case 3://quit业务
        {
            close(clientfd);
            exit(0);
            break;
        }
        default:
            cerr << "invalid input." <<endl;
            break;
        }
    }

    return 0;
}
//显示当前登录成功用户的基本信息
void showCurrentUserData()
{
    cout << "=========================login user=========================" << endl;
    cout << "current login user => id:" << g_currentUser.getId() << " name:" << g_currentUser.getName() << endl;
    cout << "-------------------------friend list------------------------" << endl;
    if (!g_currentUserFriendList.empty())
    {
        for (User &user : g_currentUserFriendList)
        {
            cout << user.getId() << " " << user.getName() << " " << user.getState() << endl;
        }
    }
    cout << "-------------------------group list------------------------" << endl;
    if (!g_currentUserGroupList.empty())
    {
        for (Group &group : g_currentUserGroupList)
        {
            cout << group.getId() << " " << group.getName() << " " << group.getDesc() << endl;
            for (GroupUser &user : group.getVecUsers())
            {
                cout << user.getId() << " " << user.getName() << " " << user.getState() << " " << user.getRole() <<endl;
            }
        }
        
    }
    
    cout << "===========================================================" << endl;
}
//获取系统时间(聊天信息需要添加时间信息)
string getCurrentTime()
{
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm *ptm = localtime(&tt);
    char date[60] = {0};
    sprintf(date, "%d-%02d-%02d %02d:%02d-%02d", (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, 
            (int)ptm->tm_mday, (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return std::string(date);
}
//接收线程
void readTaskHandler(int clientfd)
{

}
//主聊天页面程序
void mainMenu()
{

}