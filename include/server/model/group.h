#ifndef __GROUP_H__
#define __GROUP_H__

#include <string>
#include <vector>

#include "groupuser.h"

class Group
{
public:
    Group(int id = -1, std::string name = "", std::string desc = "")
    {
        this->m_id = id;
        this->m_name = name;
        this->m_desc = desc;
    }

    void set_id(int id)
    { this->m_id = id; }

    void set_name(std::string name)
    { this->m_name = name; }

    void set_desc(std::string desc)
    { this->m_desc = desc; }

    int get_id(void) const
    { return this->m_id; }

    std::string get_name(void) const
    { return this->m_name; }

    std::string get_desc(void) const
    { return this->m_desc; }

    std::vector<GroupUser>& get_users(void)
    { return this->m_users; }
private:
    int m_id;
    std::string m_name;
    std::string m_desc;
    std::vector<GroupUser> m_users;
};

#endif