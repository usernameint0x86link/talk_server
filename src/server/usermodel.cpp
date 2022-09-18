#include "usermodel.h"
#include "db.h"

bool UserModel::insert(User &user)
{
    char sql[1024] = {0};

    sprintf(sql, "INSERT INTO user(name, password, state) VALUES ('%s', '%s', '%s')", user.get_name().c_str(), user.get_password().c_str(), user.get_state().c_str());
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