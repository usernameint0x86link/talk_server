#ifndef __GROUPUSER_H__
#define __GROUPUSER_H__

#include "user.h"

class GroupUser : public User
{
public:
    void set_role(std::string role)
    { this->role = role; }
    std::string get_role() const
    { return this->role; }
private:
    std::string role;
};

#endif