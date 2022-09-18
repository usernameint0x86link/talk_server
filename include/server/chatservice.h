#ifndef __CHATSERVICE_H__
#define __CHATSERVICE_H__

#include "json.hpp"
#include "usermodel.h"

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>

using MsgHandler = std::function<void(const muduo::net::TcpConnectionPtr &, nlohmann::json&, muduo::Timestamp time)>;

// 聊天服务器业务类
class ChatService
{
public:
    // 获取单例对象的接口函数
    static ChatService* instance(void);
    // 处理登录业务
    void login(const muduo::net::TcpConnectionPtr &conn, \
    nlohmann::json &js, muduo::Timestamp timestamp);
    // 处理注册业务
    void reg(const muduo::net::TcpConnectionPtr &conn, \
    nlohmann::json &js, muduo::Timestamp timestamp);
    // 一对一聊天业务
    void oneChat(const muduo::net::TcpConnectionPtr &conn, \
    nlohmann::json &js, muduo::Timestamp timestamp);

    // 处理客户端异常退出
    void client_close_exception(const muduo::net::TcpConnectionPtr &conn);
    // 获取消息对应的处理器
    MsgHandler get_handler(int msgid);
private:
    ChatService(void);
    // 存放消息id和其对应的业务处理函数
    std::unordered_map<int, MsgHandler> m_msg_handler_map;
    // 存储在线用户的通信连接
    std::unordered_map<int, muduo::net::TcpConnectionPtr> m_user_conn_map;
    std::mutex m_conn_mutex;

    UserModel m_user_model;
};

#endif