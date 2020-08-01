#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"
#include <string>
#include <vector>
using namespace std;

//维护群组信息的接口
class GroupModel
{
public:
    //创建群组
    bool createGroup(Group& group);
    //加入群组
    bool joinGroup(int userid, int groupid, string role);
    //查询用户所在的群组信息
    vector<Group> queryGroups(int userid);
    //根据指定的groupid查询群组用户id列表，除userid自己，主要用于用户群聊给群组其他成员群发消息
    vector<int> queryGroupUsers(int userid, int groupid);
};

#endif