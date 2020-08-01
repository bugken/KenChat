#include "usermodel.hpp"
#include "db.h"
#include <iostream>

using namespace std;

//User表的插入操作
bool UserModel::insert(User &user)
{
    //组装SQL语句
    char sql[512] = {0};
    sprintf(sql, "insert into User(name, password, state)values('%s', '%s', '%s')",
            user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            //获取插入成功用户的主键ID
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }

    return false;
}

//User表的查询操作
User UserModel::query(int id)
{
    //组装SQL语句
    char sql[512] = {0};
    sprintf(sql, "select id, name, password, state from User where id = %d", id);

    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPassword(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }

    return User();
}

//更新用户的状态信息
bool UserModel::updateState(User user)
{
    //组装SQL语句
    char sql[512] = {0};
    sprintf(sql, "update User set state = '%s' where id = %d", user.getState().c_str(), user.getId());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }

    return false;
}

//充值用户状态信息
bool UserModel::resetState()
{
    //组装SQL语句
    char sql[512] = "update User set state = 'offline' where state = 'online'";

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }

    return false;
}