#include "friendmodel.hpp"
#include "db.h"
#include <iostream>

//添加好友关系
bool FriendModel::insertFriend(int userid, int friendid)
{
        //组装SQL语句
    char sql[512] = {0};
    sprintf(sql, "insert into Friend(userid, friendid)values(%d, %d)", userid, friendid);

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
//返回用户的好友列表
vector<User> FriendModel::queryFriend(int userid)
{
    char sql[512] = {0};
    sprintf(sql, "select id, name, state from User where id in (select friendid from Friend where userid = %d)", userid);

    User user;
    vector<User> vectUsers;
    vectUsers.clear();
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vectUsers.push_back(user);
            }
            mysql_free_result(res);
        }
    }

    return vectUsers;
}