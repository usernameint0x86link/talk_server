#ifndef __FRIENDMODEL_H__
#define __FRIENDMODEL_H__

#include "user.h"
#include <vector>

class FriendModel
{
public:
    void insert(int user_id, int friend_id) const;

    std::vector<User> query(int user_id) const;
};

#endif