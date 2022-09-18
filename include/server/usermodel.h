#ifndef __USERMODEL_H__
#define __USERMODEL_H__

#include "user.h"

class UserModel
{
public:
    // User表的增加方法
    bool insert(User &user);
    // 根据用户号码查询用户信息
    User query(int id);
    // 更新用户的状态信息
    bool update_state(User user);
    // 重置用户的状态信息
    void reset_state(void);
};

#endif