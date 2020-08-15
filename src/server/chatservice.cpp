#include "chatservice.hpp"
#include "public.hpp"
#include <functional>
#include <string>
#include <muduo/base/Logging.h>
#include "offlinemessagemodel.hpp"
#include "usermodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include <iostream>

using namespace std;
using namespace placeholders;
using namespace muduo;

//获取单例对象的接口函数
ChatService *ChatService::instance()
{
    static ChatService sercice;
    return &sercice;
}
//构造方法中注册消息以及对应的回调操作
ChatService::ChatService()
{
    _msgHandlerMap.insert(pair<int, MsgHandler>(LOGIN_MSG, bind(&ChatService::login, this, _1, _2, _3)));
    _msgHandlerMap.insert(pair<int, MsgHandler>(REG_MSG, bind(&ChatService::reg, this, _1, _2, _3)));
    _msgHandlerMap.insert(pair<int, MsgHandler>(LOGINOUT_MSG, bind(&ChatService::loginout, this, _1, _2, _3)));
    _msgHandlerMap.insert(pair<int, MsgHandler>(ONE_CHAT_MSG, bind(&ChatService::oneChat, this, _1, _2, _3)));
    _msgHandlerMap.insert(pair<int, MsgHandler>(ADD_FRIEND_MSG, bind(&ChatService::addFriend, this, _1, _2, _3)));
    _msgHandlerMap.insert(pair<int, MsgHandler>(CREATE_GROUP_MSG, bind(&ChatService::createGroup, this, _1, _2, _3)));
    _msgHandlerMap.insert(pair<int, MsgHandler>(JOIN_GROUP_MSG, bind(&ChatService::joinGroup, this, _1, _2, _3)));
    _msgHandlerMap.insert(pair<int, MsgHandler>(GROUP_CHAT_MSG, bind(&ChatService::groupChat, this, _1, _2, _3)));

    //连接redis服务器
    if (_redis.connect())
    {
        //设置上报消息的回调
        _redis.init_notify_handler(bind(&ChatService::handleRedisSubscibeMessage, this, _1, _2));
    }
    
}
//获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid)
{
    //记录错误日志 msgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time) {
            LOG_ERROR << "msgid " << msgid << " not found handler!";
        };
    }
    return _msgHandlerMap[msgid];
}
//客户端异常断开时处理函数
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                user.setId(it->first);
                //删除长连接
                _userConnMap.erase(it);
                LOG_INFO << "handle done after disconnection.";
                break;
            }
        }
    }

    //更新用户状态
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }

    //unsubscribe用户对应的channel(id)
    _redis.unsubscribe(user.getId());
}
//服务器异常业务充值方法
void ChatService::reset()
{
    //把所有用户的状态设置为offline
    _userModel.resetState();
}
//一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    TcpConnectionPtr clientConn;
    int toid = js["toid"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            //toid在线，转发消息，服务器主动推送消息给toid用户
            LOG_INFO << "transfer information.";
            it->second->send(js.dump());
            return;
        }
    }

    //查询toid是否在线
    User user = _userModel.query(toid);
    if (user.getState() == "online")
    {
        _redis.publish(user.getId(), js.dump());
        return;
    }
    
    //toid不在线，存储离线消息
    LOG_INFO << "transfer offline information.";
    _offlineMsgModel.insert(toid, js.dump());
}
//处理登录业务
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    LOG_INFO << "do login service.";
    json response;
    int id = js["id"].get<int>();
    string pwd = js["password"].get<string>();
    User user = _userModel.query(id);
    if (user.getId() == id && user.getPassword() == pwd)
    {
        if (user.getState() == "online")
        {
            //该用户已经在线，不允许重复登录
            LOG_INFO << "login failed.login already.";
            response["errno"] = 1;
            response["errmsg"] = "登录失败，该账号已经登录.";
        }
        else
        {
            LOG_INFO << "login success.";
            {
                //登录成功，保存用户的长连接
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert(pair<int, TcpConnectionPtr>(user.getId(), conn));
            }

            //id用户登录成功后，向redis订阅channel(id)
            _redis.subscribe(user.getId());

            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            //登录成功，更新用户状态
            user.setState("online");
            _userModel.updateState(user);
        }
    }
    else
    {
        LOG_INFO << "login failed.user not exist.";
        response["errno"] = 1;
        response["errmsg"] = "登录失败，用户不存在.";
    }

    response["msgid"] = LOGIN_MSG_ACK;
    sendWithHttp(conn, response.dump());
}
//处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    LOG_INFO << "do reg service.";
    string name = js["name"];
    string pwd = js["password"];
    User user;
    json response;
    user.setName(name);
    user.setPassword(pwd);
    bool ret = _userModel.insert(user);
    if (ret)
    {
        //注册成功
        response["errno"] = 0;
        response["id"] = user.getId();
    }
    else
    {
        //注册失败
        response["errno"] = 1;
        response["errmsg"] = "注册失败";
    }

    response["msgid"] = REG_MSG_ACK;
    sendWithHttp(conn, response.dump());
}
//注销消息
void ChatService::loginout(const TcpConnectionPtr & conn, json &js, Timestamp time)
{
    int userid = js["userid"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
            //删除长连接
            _userConnMap.erase(it);
            LOG_INFO << "user " << userid <<" loginout.";
        }
    }

    //更新状态
    User user(userid, "", "", "offline");
    _userModel.updateState(user);

    //unsubscribe用户对应的channel(id)
    _redis.unsubscribe(userid);
}
//添加好友
void ChatService::addFriend(const TcpConnectionPtr & conn, json &js, Timestamp time)
{
    int userid = js["userid"].get<int>();
    int friendid = js["friendid"].get<int>();

    if(_friendModel.insertFriend(userid, friendid))
    {
        LOG_INFO << "添加好友成功.";
    }
    else
    {
        LOG_INFO << "添加好友失败.";
    }
}
//创建群组
void ChatService::createGroup(const TcpConnectionPtr & conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    Group groupInfo(-1, js["groupname"], js["groupdesc"]);

    if(_groupModel.createGroup(groupInfo))
    {
        LOG_INFO << "创建群组成功.";
        //存储群组的创建人信息
        _groupModel.joinGroup(userid, groupInfo.getId(), "creator");
    }
    else
    {
        LOG_INFO << "创建群组失败.";
    }
}
//加入群组
void ChatService::joinGroup(const TcpConnectionPtr & conn, json &js, Timestamp time)
{
    int userid = js["userid"].get<int>();
    int groupid = js["groupid"].get<int>();

    if(_groupModel.joinGroup(userid, groupid, "normal"))
    {
        LOG_INFO << "加入群组成功.";
    }
    else
    {
        LOG_INFO << "加入群组失败.";
    }
}
//群聊
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["userid"].get<int>();
    int groupid = js["groupid"].get<int>();

    vector<int> vecUsers = _groupModel.queryGroupUsers(userid, groupid);
    if (!vecUsers.empty())
    {
        lock_guard<mutex> lock(_connMutex);
        for (int userid : vecUsers)
        {
            auto it = _userConnMap.find(userid);
            if (it != _userConnMap.end())
            {
                //userid在线，转发消息，服务器主动推送消息给toid用户
                LOG_INFO << "transfer information.";
                it->second->send(js.dump());
                continue;
            }

            //查询toid是否在线
            User user = _userModel.query(userid);
            if (user.getState() == "online")
            {
                _redis.publish(user.getId(), js.dump());
                return;
            }
            //userid不在线，存储离线消息
            LOG_INFO << "transfer offline information.";
            _offlineMsgModel.insert(userid, js.dump());
        }
    }
}
//从redis消息队列中取订阅的消息
void ChatService::handleRedisSubscibeMessage(int userid, string msg)
{
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
            it->second->send(msg);
            return ;
        }
    }
    //存储该用户的离线消息
    _offlineMsgModel.insert(userid, msg);
}
//封装成http消息发出
void ChatService::sendWithHttp(const TcpConnectionPtr &conn, string jsonBuff)
{
    string httpResponse;
    HttpHandle::instance()->buildHttpResponse(jsonBuff, httpResponse);
    
    conn->send(httpResponse);
}