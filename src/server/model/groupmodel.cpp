#include "groupmodel.h"
#include "db.h"

bool GroupModel::create_group(Group &group) const
{
    char sql[1024] = { 0 };
    sprintf(sql, "INSERT INTO allgroup(groupname, groupdesc) VALUES('%s','%s')", group.get_name().c_str(), group.get_desc().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if(mysql.update(sql))
        {
            group.set_id(mysql_insert_id(mysql.get_connection()));
            return true;
        }
    }
    return false;
}

void GroupModel::add_group(int user_id, int group_id, std::string role) const
{
    char sql[1024] = { 0 };
    sprintf(sql, "INSERT INTO groupuser VALUES(%d, %d, '%s')", group_id, user_id, role.c_str());

    MySQL mysql;
    if (mysql.connect())
    { mysql.update(sql); }
}

std::vector<Group> GroupModel::query_groups(int user_id) const
{
    char sql[1024] = { 0 };
    sprintf(sql, "SELECT a.id,a.groupname, a.groupdesc from allgroup a inner join groupuser b on a.id=b.groupid where b.userid=%d", user_id);

    std::vector<Group> group_vec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                Group group;
                group.set_id(atoi(row[0]));
                group.set_name(row[1]);
                group.set_desc(row[2]);
                group_vec.push_back(group);
            }
            mysql_free_result(res);
        }
    }
    return group_vec;
}

std::vector<int> GroupModel::query_group_users(int user_id, int group_id) const
{
    char sql[1024] = {0};
    sprintf(sql, "SELECT userid FROM groupuser WHERE groupid=%d and uerid != %d", group_id, user_id);

    std::vector<int> vec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            { vec.push_back(atoi(row[0])); }
            mysql_free_result(res);
        }
    }
    return vec;
}