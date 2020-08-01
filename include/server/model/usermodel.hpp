#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"

//User表的数据操作类
class UserModel
{
public:
    //User表的插入操作
    bool insert(User &user);
    //User表的查询操作
    User query(int id);
    //更新用户的状态信息
    bool updateState(User user);
    //充值用户状态信息
    bool resetState();
};

#endif