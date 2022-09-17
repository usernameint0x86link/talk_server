#ifndef __CHATSERVICE_H__
#define __CHATSERVICE_H__

#include "json.hpp"

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>

using MsgHandler = std::function<void(const muduo::net::TcpConnectionPtr &, nlohmann::json*, muduo::Timestamp time)>;

// 聊天服务器业务类
class ChatService
{
public:
    // 获取单例对象的接口函数
    static ChatService* instance(void);
    // 处理登录业务
    void login(const muduo::net::TcpConnectionPtr &conn, \
    nlohmann::json *js, muduo::Timestamp timestamp);
    // 处理注册业务
    void reg(const muduo::net::TcpConnectionPtr &conn, \
    nlohmann::json *js, muduo::Timestamp timestamp);

    MsgHandler get_handler(int msgid);
private:
    ChatService(void);
    // 存放消息id和其对应的业务处理函数
    std::unordered_map<int, MsgHandler> m_msg_handler_map;
};

#endif