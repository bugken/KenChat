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
#include "http.hpp"

using namespace std;
using json=nlohmann::json;

//记录当前系统登录的用户信息
User g_currentUser;
//记录当前登录用户的好友列表信息
vector<User> g_currentUserFriendList;
//记录当前登录用户的群组列表信息
vector<Group> g_currentUserGroupList;
//控制聊天主界面
bool isMainMenuRuning = false;
//控制读写成只启动一次
bool isReadThreadStartUp = false;
//显示当前登录成功用户的基本信息
void showCurrentUserData();
//接收线程
void readTaskHandler(int clientfd);
//获取系统时间(聊天信息需要添加时间信息)
string getCurrentTime();
//封装成http包并发送
int sendWithHttp(int clientfd, string& jsonBuff);
//解析收到的http包
int recvParser(int clientfd, string& jsonBuffer);
//主聊天页面程序
void mainMenu(int clientfd);
//help command handler
void help(int fd, string msg);
//chat command handler
void chat(int clientfd, string msg);
//addfriend command handler
void addfriend(int clientfd, string msg);
//creategroup command handler
void creategroup(int clientfd, string msg);
//joingroup command handler
void joingroup(int clientfd, string msg);
//groupchat command handler
void groupchat(int clientfd, string msg);
//loginout command handler
void loginout(int clientfd, string msg);
//系统支持的客户端命令列表
unordered_map<string, string> commandMap = 
{
    {"help", "显示所有支持的命令，格式help"},
    {"chat", "一对一聊天，格式chat:friendid:message"},
    {"addfriend", "添加好友，格式addfriend:friendid"},
    {"creategroup", "创建群组，格式creategroup:groupname:groupdesc"},
    {"joingroup", "加入群组，格式joingroup:groupid"},
    {"groupchat", "群聊，格式groupchat:groupid:message"},
    {"loginout", "注销，格式loginout"},
};
//注册系统支持的客户端命令处理
unordered_map<string, function<void(int, string)>> commandHandlerMap = 
{
    {"help", help},
    {"chat", chat},
    {"addfriend", addfriend},
    {"creategroup", creategroup},
    {"joingroup", joingroup},
    {"groupchat", groupchat},
    {"loginout", loginout},
};

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
        char input;
        char throwaway[100] = {0};
        cin.get(input);
        if (!isdigit(input))
        {
            continue;
        }
        int choice = (int)input - 48;
        cin.getline(throwaway, 50);//将剩下的字符读取扔掉，否则会影响业务的输入
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

            int len = sendWithHttp(clientfd, request);
            if (-1 == len)
            {
                cerr << "send login msg error." << endl;
            }
            else
            {
                string jsonBuffer;
                len = recvParser(clientfd, jsonBuffer);
                if (-1 == len)
                {
                    cerr << "recv login response error." << endl;
                }
                else
                {
                    json response = json::parse(jsonBuffer);
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
                            g_currentUserFriendList.clear();
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
                            g_currentUserGroupList.clear();
                            for (string &str : vecGroups)
                            {
                                jsgroups = json::parse(str);
                                group.setId(jsgroups["id"].get<int>());
                                group.setName(jsgroups["groupname"]);
                                group.setDesc(jsgroups["groupdesc"]);

                                if (jsgroups.contains("users"))
                                {
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
                                if (ONE_CHAT_MSG == jsOfflineMessage["msgid"].get<int>())
                                {
                                    cout << jsOfflineMessage["time"].get<string>() << " [" << jsOfflineMessage["id"] 
                                        << "]" << jsOfflineMessage["name"].get<string>() << " said:" 
                                        << jsOfflineMessage["message"].get<string>() << endl;
                                }
                                else
                                {
                                    cout << "群消息[" << jsOfflineMessage["groupid"] << "]" << jsOfflineMessage["time"].get<string>() 
                                        << " [" << jsOfflineMessage["id"] << "]" << jsOfflineMessage["name"].get<string>() 
                                        << " said:" << jsOfflineMessage["message"].get<string>() << endl;
                                }
                            }
                        }

                        //登录成功后，启动接收线程负责接收数据，该线程只启动一次
                        if (false == isReadThreadStartUp)
                        {
                            std::thread readTask(readTaskHandler, clientfd);//C++11语言级创建线程函数，相当于pthread_create
                            readTask.detach();//相当于pthread_detach
                            isReadThreadStartUp = true;
                        }
                        
                        //进入聊天主页面
                        isMainMenuRuning = true;
                        mainMenu(clientfd);
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

            int len = sendWithHttp(clientfd, request);
            if (len == -1)
            {
                cerr << "send reg msg error:" << request << endl;
            }
            else
            {
                string jsonBuffer;
                len = recvParser(clientfd, jsonBuffer);
                if(len == -1)
                {
                    cerr << "recv reg response error." << endl;
                }
                else
                {
                    json response = json::parse(jsonBuffer);
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
                cout << user.getId() << " " << user.getName() << " " << user.getState() << " " << user.getRole() << endl;
            }
        }
        
    }
    
    cout << "===========================================================" << endl;
}
//help command handler
void help(int fd = 0, string str = "")
{
    cout << "show command list >>>" << endl;
    for (auto &cmd : commandMap)
    {
        cout << cmd.first << " : " << cmd.second << endl;
    }
}
//chat command handler chat:friendid:message
void chat(int clientfd, string msg)
{
    int idx = msg.find(":");
    if (-1 == idx)
    {
        cerr << "chat command invalid." << endl;
        return ;
    }

    int friendid = atoi(msg.substr(0, idx).c_str());
    string message = msg.substr(idx + 1, msg.size() - idx);

    json context;
    context["msgid"] = ONE_CHAT_MSG;
    context["id"] = g_currentUser.getId();
    context["name"] = g_currentUser.getName();
    context["toid"] = friendid;
    context["message"] = message;
    context["time"] = getCurrentTime();
    string buffer = context.dump();

    int len = sendWithHttp(clientfd, buffer);
    if (-1 == len)
    {
        cerr <<"send chat msg error -> " << buffer << endl;
        return;
    }
}
//addfriend command handler addfriend:friendid
void addfriend(int clientfd, string msg)
{
    int friendid = atoi(msg.c_str());
    json context;
    context["msgid"] = ADD_FRIEND_MSG;
    context["userid"] = g_currentUser.getId();
    context["friendid"] = friendid;
    string buffer = context.dump();

    int len = sendWithHttp(clientfd, buffer);
    if(-1 == len)
    {
        cerr << "send addfriend msg error -> " << buffer << endl;
    }
}
//creategroup command handler creategroup:groupname:groupdesc
void creategroup(int clientfd, string msg)
{
    int idx = msg.find(":");
    if (-1 == idx)
    {
        cerr << "create group command invalid." << endl;
        return ;
    }
    string groupname = msg.substr(0, idx);
    string groupdesc = msg.substr(idx + 1, msg.size() - idx);

    json context;
    context["msgid"] = CREATE_GROUP_MSG;
    context["id"] = g_currentUser.getId();
    context["groupname"] = groupname;
    context["groupdesc"] = groupdesc;
    context["time"] = getCurrentTime();
    string buffer = context.dump();

    int len = sendWithHttp(clientfd, buffer);
    if (-1 == len)
    {
        cerr <<"send creategroup msg error -> " << buffer << endl;
        return;
    }
}
//joingroup command handler groupid
void joingroup(int clientfd, string msg)
{
    int groupid = atoi(msg.c_str());

    json context;
    context["msgid"] = JOIN_GROUP_MSG;
    context["userid"] = g_currentUser.getId();
    context["groupid"] = groupid;
    context["time"] = getCurrentTime();
    string buffer = context.dump();

    int len = sendWithHttp(clientfd, buffer);
    if (-1 == len)
    {
        cerr <<"send joingroup msg error -> " << buffer << endl;
        return;
    }
}
//groupchat command handler groupid:message
void groupchat(int clientfd, string msg)
{
    int idx = msg.find(":");
    if (-1 == idx)
    {
        cerr << "groupchat command invalid." << endl;
        return ;
    }
    int groupid = atoi(msg.substr(0, idx).c_str());
    string message = msg.substr(idx + 1, msg.size() - idx);

    json context;
    context["msgid"] = GROUP_CHAT_MSG;
    context["userid"] = g_currentUser.getId();
    context["name"] = g_currentUser.getName();
    context["groupid"] = groupid;
    context["message"] = message;
    context["time"] = getCurrentTime();
    string buffer = context.dump();

    int len = sendWithHttp(clientfd, buffer);
    if (-1 == len)
    {
        cerr <<"send groupchat msg error -> " << buffer << endl;
        return;
    }
}
//loginout command handler
void loginout(int clientfd, string msg)
{
    json context;
    context["msgid"] = LOGINOUT_MSG;
    context["userid"] = g_currentUser.getId();
    context["time"] = getCurrentTime();
    string buffer = context.dump();

    int len = sendWithHttp(clientfd, buffer);
    if (-1 == len)
    {
        cerr <<"send loginout msg error -> " << buffer << endl;
        return;
    }

    //关闭用户聊天主界面
    isMainMenuRuning = false;
}
//获取系统时间(聊天信息需要添加时间信息)
string getCurrentTime()
{
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm *ptm = localtime(&tt);
    char date[60] = {0};
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d", (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, 
            (int)ptm->tm_mday, (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return std::string(date);
}
//接收线程
void readTaskHandler(int clientfd)
{
    for (;;)
    {
        string jsonBuffer;
        int len = recvParser(clientfd, jsonBuffer);
        if (-1 == len || 0 == len)
        {
            close(clientfd);
            exit(-1);
        }
        //接收ChatServer转发的数据，反序列化生成json数据对象
        json js = json::parse(jsonBuffer);
        if (ONE_CHAT_MSG == js["msgid"].get<int>())
        {
            cout << js["time"].get<string>() << " [" << js["id"] << "]" 
                << js["name"].get<string>() << " said:" << js["message"].get<string>() << endl;
            continue;
        }
        if (GROUP_CHAT_MSG == js["msgid"].get<int>())
        {
            cout << "群消息[" << js["groupid"] << "]" << js["time"].get<string>() << " [" << js["userid"].get<int>() << "]" 
                << js["name"].get<string>() << " said:" << js["message"].get<string>() << endl;
            continue;
        }
    }
}
//封装成http包并发送
int sendWithHttp(int clientfd, string& jsonBuff)
{
    string httpResponse;
    HttpHandle::instance()->buildHttpResquest(jsonBuff, httpResponse);
    int len = send(clientfd, httpResponse.c_str(), strlen(httpResponse.c_str()), 0);

    return len;
}
//解析收到的http包
int recvParser(int clientfd, string& jsonBuffer)
{
    char buffer[1024] = {0};
    int len = recv(clientfd, buffer, 1024, 0);
    string recvBuff = string(buffer);
    HttpHandle::instance()->parseHttpMessage(recvBuff, jsonBuffer);  
    return len;
}
//主聊天页面程序
void mainMenu(int clientfd)
{
    help();
    char buffer[1024] = {0};
    while (isMainMenuRuning)
    {
        cin.getline(buffer, 1024);
        string commandBuf(buffer);
        string command;//存储命令
        int idx = commandBuf.find(":");
        if (-1 == idx)
        {
           command = commandBuf;
        }
        else
        {
            command = commandBuf.substr(0, idx);
        }
        auto it = commandHandlerMap.find(command);
        if (it == commandHandlerMap.end())
        {
            cerr << "invalid input command." << endl;
            continue;
        }

        //调用相应的命令处理函数，mainMenu对修改封闭，添加新功能不需要修改该函数
        it->second(clientfd, commandBuf.substr(idx + 1, commandBuf.size() - idx));
    }
}