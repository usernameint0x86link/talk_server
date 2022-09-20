#include "friendmodel.h"
#include "db.h"

void FriendModel::insert(int user_id, int friend_id) const
{
    char sql[1024] = { 0 };
    sprintf(sql, "INSERT INTO friend VALUES (%d, %d)", user_id, friend_id);

    MySQL mysql;
    if (mysql.connect())
    { mysql.update(sql); }
}

std::vector<User> FriendModel::query(int user_id) const
{
    char sql[1024] = { 0 };
    sprintf(sql, "SELECT a.id, a.name, a.state from user a inner join friend b on b.friendid = a.id where b.userid=%d", user_id);

    std::vector<User> vec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.set_id(atoi(row[0]));
                user.set_name(row[1]);
                user.set_state(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}