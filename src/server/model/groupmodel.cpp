#include "groupmodel.hpp"
#include "db.h"

//创建群组
bool GroupModel::createGroup(Group &group)
{
    //组装SQL语句
    char sql[512] = {0};
    sprintf(sql, "insert into AllGroup(groupname, groupdesc)values('%s', '%s')",
            group.getName().c_str(), group.getDesc().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            //获取插入成功用户群的主键ID
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }

    return false;
}
//加入群组
bool GroupModel::joinGroup(int userid, int groupid, string role)
{
    //组装SQL语句
    char sql[512] = {0};
    sprintf(sql, "insert into GroupUser(userid, groupid, grouprole)values(%d, %d, '%s')",
                 userid, groupid, role.c_str());

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
//查询用户所在的群组信息
vector<Group> GroupModel::queryGroups(int userid)
{
    char sql[512] = {0};
    sprintf(sql, "select id, groupname, groupdesc from AllGroup where id in \
                (select groupid from GroupUser where userid = %d)", userid);

    Group group;
    vector<Group> vecGroup;
    vecGroup.clear();
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                vecGroup.push_back(group);
            }
            mysql_free_result(res);
        }
    }

    //查询群组的用户信息
    GroupUser groupUser;
    for (Group& group : vecGroup)
    {
        sprintf(sql, "select a.id, a.name, a.state from User a inner join GroupUser b \
                        on a.id = b.userid where b.groupid = %d;", group.getId());

        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                groupUser.setId(atoi(row[0]));
                groupUser.setName(row[1]);
                groupUser.setState(row[2]);
                groupUser.setRole(row[3]);
                group.getVecUsers().push_back(groupUser);
            }
            mysql_free_result(res);
        }
    }
    
    return vecGroup;
}
//根据指定的groupid查询群组用户id列表，除userid自己，主要用于用户群聊给群组其他成员群发消息
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    char sql[512] = {0};
    sprintf(sql, "select userid from GroupUser where groupid = %d and userid != %d;",
                    groupid, userid);

    Group group;
    vector<int> vecUsers;
    vecUsers.clear();
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                vecUsers.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }

    return vecUsers;
}