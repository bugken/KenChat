/*
muduo网络库给用户提供了两个主要的类
TcpServer:用于编写服务器程序
TcpClient:用于编写客户端程序
epoll + 线程池
好处:能够把网络I/O的代码和业务代码区分开来，程序员只关注业务代码编程，而I/O相关的操作交给网络库来操作
    而业务代码程序员只需要关注:用户的连接和断开以及用户的可读写事件
*/

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
#include <string>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

/*
基于muduo网络库开发服务器程序
1.组合TcpServer对象
2.创建EventLoop事件循环对象的指针
3.明确TCPServer构造函数需要什么参数,输出ChatServer的构造函数
4.在当前服务器类的构造函数中，注册处理连接的回调函数和处理读写事件的回调函数
5.设置服务器端的线程数量，muduo库会自己划分I/O线程和工作线程
*/
class ChatServer
{
public:
    ChatServer(EventLoop *loop,               //事件循环
               const InetAddress &listenAddr, //IP+Port
               const string &nameArg)         //服务器名称
        : _server(loop, listenAddr, nameArg), _loop(loop)
    {
        //给服务器注册用户连接的创建和断开回调
        _server.setConnectionCallback(bind(&ChatServer::onConnection, this, _1));
        //给服务器注册用户读写事件的回调
        _server.setMessageCallback(bind(&ChatServer::onMessage, this, _1, _2, _3));
        //设置服务器端的线程数量 1个I/O线程和1个工作线程
        _server.setThreadNum(2);
    }

    //开启事件循环
    void start()
    {
        _server.start();
    }

private:
    //专门处理用户的连接创建和端口
    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << " stat:online" << endl;
        }
        else
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << " stat:offline" << endl;
            conn->shutdown();
            //_loop->quit();
        }
    }
    //专门处理用户的读写事件
    void onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data:" << buf << "time:" << time.toString() <<endl;
        conn->send(buf);
    }

    TcpServer _server; //#1
    EventLoop *_loop;  //#2
};

int main()
{
    EventLoop loop;//epoll
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");
    server.start();//listen epoll_ctl
    loop.loop();//epoll_wait以阻塞方式等待用户连接和用户读写事件

    return 0;
}