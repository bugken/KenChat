#ifndef PUBLIC_H
#define PUBLIC_H

/*server和client的公共文件*/
enum EnMsgType
{
    NO_ERROR_MSG = 0,      //无错误消息以及来自WebClient的保活消息
    LOGIN_MSG = 1,      //登录消息
    LOGIN_MSG_ACK,
    REG_MSG,            //注册消息
    REG_MSG_ACK,        //注册响应消息
    LOGINOUT_MSG,       //注销消息
    ONE_CHAT_MSG,       //一对一聊天消息
    ADD_FRIEND_MSG,     //添加好友信息

    CREATE_GROUP_MSG,   //创建群组
    JOIN_GROUP_MSG,     //加入群组
    GROUP_CHAT_MSG,     //群聊天

    OFFLINE_MSG,        //离线消息
    OFFLINE_MSG_ACK,    //离线消息回复消息

    FRIENDS_MSG,        //好友消息
    FRIENDS_MSG_ACK,

    GROUPS_MSG,         //群组消息
    GROUPS_MSG_ACK,     
};

#endif