#ifndef __USER_H__
#define __USER_H__

#include <string>

// 
class User
{
public:
    User(int id = -1, std::string name = "", std::string password = "", \
    std::string state = "offline")
    {
        m_id = id;
        m_name = name;
        m_password = password;
        m_state = state;
    }
    void set_id(int id)
    { m_id = id; }
    void set_name(std::string name)
    { m_name = name; }
    void set_password(std::string password)
    { m_password = password; }
    void set_state(std::string state)
    { m_state = state; }

    int get_id(void) const 
    { return m_id; }
    std::string get_name(void) const
    { return m_name; }
    std::string get_password(void) const
    { return m_password; }
    std::string get_state(void) const
    { return m_state; }
private:
    int m_id;
    std::string m_name;
    std::string m_password;
    std::string m_state;
};


#endif