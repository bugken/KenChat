#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <muduo/net/EventLoop.h>
#include <unordered_map>
#include <functional>
#include "json.hpp"
#include "usermodel.hpp"
#include "redis.hpp"
#include "http.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include <mutex>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;

//处理消息的时间回调方法类型
using MsgHandler = function<void (const TcpConnectionPtr & conn, json &js, Timestamp time)>;

//聊天服务器业务类
class ChatService
{
public:
    //获取单例对象的接口函数
    static ChatService* instance();
    //处理登录业务
    void login(const TcpConnectionPtr & conn, json &js, Timestamp time);
    //处理注册业务
    void reg(const TcpConnectionPtr & conn, json &js, Timestamp time);
    //注销消息
    void loginout(const TcpConnectionPtr & conn, json &js, Timestamp time);
    //一对一聊天业务
    void oneChat(const TcpConnectionPtr & conn, json &js, Timestamp time); 
    //添加好友
    void addFriend(const TcpConnectionPtr & conn, json &js, Timestamp time);
    //创建群组
    void createGroup(const TcpConnectionPtr & conn, json &js, Timestamp time);
    //加入群组
    void joinGroup(const TcpConnectionPtr & conn, json &js, Timestamp time);
    //群聊
    void groupChat(const TcpConnectionPtr & conn, json &js, Timestamp time);
    //从redis消息队列中取订阅的消息
    void handleRedisSubscibeMessage(int userid, string msg);
    //获取消息对应的处理器
    MsgHandler getHandler(int msgid);
    //客户端异常断开时处理函数
    void clientCloseException(const TcpConnectionPtr & conn);
    //服务器异常业务充值方法
    void reset();
    //封装成http消息发出
    void sendWithHttp(const TcpConnectionPtr &conn, string jsonBuff);
private:
    ChatService();
    //存储消息ID和其对应的业务处理方法，此成员不需要线程安全，因为启动时候已经初始化好
    unordered_map<int, MsgHandler> _msgHandlerMap;
    //存储在线用户的通信连接,此程序需要线程安全
    unordered_map<int, TcpConnectionPtr> _userConnMap;
    //定义互斥所，保证_userConnMap线程安全
    mutex _connMutex;

    //数据操作类对象
    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;

    //Redis操作对象
    Redis _redis;
};

#endif