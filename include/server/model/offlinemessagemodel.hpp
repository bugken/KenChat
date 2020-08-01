#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H

#include <vector>
#include <string>

using namespace std;

//提供离线消息表的接口方法
class OfflineMsgModel
{
public:
    //存储用户的离线消息
    bool insert(int userid, string msg);
    //删除用户的离线消息
    bool remove(int userid);
    //读取用户的离线消息
    vector<string> query(int userid);
};

#endif