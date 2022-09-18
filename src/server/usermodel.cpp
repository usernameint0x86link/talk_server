#include "usermodel.h"
#include "db.h"

// User表的增加方法
bool UserModel::insert(User &user)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "INSERT INTO user(name, password, state) VALUES ('%s', '%s', '%s')", \
    user.get_name().c_str(), user.get_password().c_str(), \
    user.get_state().c_str());
   
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        { 
            user.set_id(mysql_insert_id(mysql.get_connection()));
            return true; 
        }
    }

    return false;
}

bool UserModel::update_state(User user)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "UPDATE user SET state = '%s' WHERE id = %d", \
    user.get_state().c_str(), user.get_id());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        { return true; }
    }

    return false;
}

User UserModel::query(int id)
{
    char sql[1024] = {0};

    sprintf(sql, "SELECT * FROM user WHERE id = %d", id);
    MySQL mysql;

    if (mysql.connect())
    {
        MYSQL_RES* res =  mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.set_id(std::atoi(row[0]));
                user.set_name(row[1]);
                user.set_password(row[2]);
                user.set_state(row[3]);

                mysql_free_result(res);
                return user;
            }
        }
    }

    return User();
}