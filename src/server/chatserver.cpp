#include "chatserver.hpp"
#include "chatservice.hpp"
#include "json.hpp"
#include "http.hpp"
#include <iostream>
#include <functional>
#include <string>
#include <muduo/base/Logging.h>

using namespace std;
using namespace placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg) 
        : _server(loop, listenAddr, nameArg), _loop(loop)
{
    //注册链接事件回调函数
    _server.setConnectionCallback(bind(&ChatServer::OnConnection, this, _1));
    //注册读写事件回调函数
    _server.setMessageCallback(bind(&ChatServer::OnMessage, this, _1, _2, _3));
    //设置线程数量
    _server.setThreadNum(2);
}

//启动服务
void ChatServer::start()
{
    _server.start();

    return ;
}

//上报链接相关信息的回调函数
void ChatServer::OnConnection(const TcpConnectionPtr &conn)
{
    //客户端断开连接
    if (!conn->connected())
    {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
    
    return;
}
//上报读写事件相关信息的回调函数
 void ChatServer::OnMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
 {
     string jsonBuff;
     string receiveBuf = buffer->retrieveAllAsString();
     HttpHandle::instance()->parseHttpMessage(receiveBuf, jsonBuff);
     //数据反序列化
     json js;
     try
     {
        js = json::parse(jsonBuff);
     }
     catch(json::exception& e)
     {
        LOG_ERROR << "json parse error and skip this msg.";
        return;
     }
     
     //目的:完全解耦网络模块代码和业务模块代码,通过js["msgid"]获取一个业务处理器handler
     auto it = js.find("msgid");
     if (it != js.end())
     {
        auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
        //回调消息绑定的时间处理器来执行相应的业务处理
        msgHandler(conn, js, time);
     }
     else
     {
        LOG_ERROR << "json info has no msgid and skip this msg.";
     }
 }

