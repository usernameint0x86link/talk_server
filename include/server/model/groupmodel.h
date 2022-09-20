#ifndef __GROUPMODEL_H__
#define __GROUPMODEL_H__

#include "group.h"

#include <string>
#include <vector>

class GroupModel
{
public:
    bool create_group(Group &group) const;
    void add_group(int user_id, int group_id, std::string role) const;
    std::vector<Group> query_groups(int user_id) const;
    std::vector<int> query_group_users(int user_id, int group_id) const;
};

#endif