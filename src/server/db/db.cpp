#include "db.h"

#include <muduo/base/Logging.h>

// 数据库的配置信息
static std::string server_ip = "127.0.0.1";
static const int port = 3306;
static std::string user = "root";
static std::string password = "123456";
static std::string db_name = "chat";

// 初始化数据库连接
MySQL::MySQL(void)
{ m_conn = mysql_init(nullptr); }

// 释放数据库连接
MySQL::~MySQL(void)
{
    if (m_conn != nullptr)
    { mysql_close(m_conn); }
}

// 连接数据库
bool MySQL::connect(void)
{
    MYSQL *p = mysql_real_connect(m_conn, server_ip.c_str(), user.c_str(), \
        password.c_str(), db_name.c_str(), port, nullptr, 0);
    if (p != nullptr)
    { 
        mysql_query(m_conn, "set names gbk"); 
        LOG_INFO << "connect mysql successfully";
    }
    else
    { LOG_INFO << "connect mysql failed"; }


    return p != nullptr;
}

// 更新操作
bool MySQL::update(std::string sql)
{
    if (mysql_query(m_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << __LINE__ << ":" << sql << "update failed";
        return false;
    }

    return true;
}

// 查询操作
MYSQL_RES* MySQL::query(std::string sql)
{
    if (mysql_query(m_conn, sql.c_str()) != 0)
    {
        LOG_INFO << __FILE__ << __LINE__ << ":" << sql << "query failed";
        return nullptr;
    }

    return mysql_use_result(m_conn);
}

MYSQL* MySQL::get_connection(void)
{
    return m_conn;
}