#ifndef __PUBLIC_H__
#define __PUBLIC_H__

enum EnMsgType
{
    LOGIN_MSG = 1, // 登录信息
    LOGIN_MSG_ACK, // 登录响应信息
    LOGINOUT_MSG, // 注销消息
    REG_MSG = 4, // 注册信息
    REG_MSG_ACK, // 注册响应信息
    ONE_CHAT_MSG, // 聊天消息
    ADD_FRIEND_MSG, // 添加好友消息

    GREATE_GROUP_MSG,
    ADD_GROUP_MSG,
    GROUP_CHAT_MSG
};

#endif