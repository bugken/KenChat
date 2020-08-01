#include "db.h"
#include <muduo/base/Logging.h>

using namespace std;

//数据库配置信息
static string server = "localhost";
static string user = "root";
static string password = "123456";
static string dbname = "chat";

//初始化数据库连接
MySQL::MySQL()
{
    _conn = mysql_init(nullptr);
}
//释放数据库连接资源
MySQL::~MySQL()
{
    if (_conn != nullptr)
    {
        mysql_close(nullptr);
    }
}
//连接数据库
bool MySQL::connect()
{
    MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(), password.c_str(), dbname.c_str(), 3306, nullptr, 0);
    if (p != nullptr)
    {
        //C和C++代码默认字符编码是ASCII，如果不设置从MySQL上拉下来的中文显示为？
        mysql_query(_conn, "set names gbk");
        LOG_INFO << "connect mysql sucess.";
    }
    else
    {
        LOG_INFO << mysql_error(_conn);
        LOG_INFO << "connect mysql failed.";
    }
    
    return p;        
}
//更新操作
bool MySQL::update(const char* sql)
{
    if (mysql_query(_conn, sql))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << " 更新失败";
        return false;
    }
    return true;
}
//查询操作
MYSQL_RES* MySQL::query(const char* sql)
{
    if (mysql_query(_conn, sql))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << " 查询失败";
        return nullptr;
    }
    return mysql_use_result(_conn);
}
//获取连接
MYSQL* MySQL::getConnection()
{
    return _conn;
}
