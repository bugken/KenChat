#include "offlinemessagemodel.hpp"
#include "db.h"
#include <iostream>

//存储用户的离线消息
bool OfflineMsgModel::insert(int userid, string msg)
{
    //组装SQL语句
    char sql[512] = {0};
    sprintf(sql, "insert into OfflineMessage(userid, message)values(%d, '%s')", userid, msg.c_str());

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
//删除用户的离线消息
bool OfflineMsgModel::remove(int userid)
{
    //组装SQL语句
    char sql[512] = {0};
    sprintf(sql, "delete from OfflineMessage where userid = %d", userid);

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
//读取用户的离线消息
vector<string> OfflineMsgModel::query(int userid)
{
    char sql[512] = {0};
    sprintf(sql, "select message from OfflineMessage where userid = %d", userid);

    vector<string> vectMessage;
    vectMessage.clear();
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                vectMessage.push_back(row[0]);
            }
            mysql_free_result(res);
        }
    }

    return vectMessage;
}