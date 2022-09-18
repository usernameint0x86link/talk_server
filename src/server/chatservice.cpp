#include "chatservice.h"
#include "public.h"

#include <functional>
#include <muduo/base/Logging.h>

using namespace std;
using namespace placeholders;

ChatService* ChatService::instance(void)
{
    static ChatService service;
    return &service;
}

ChatService::ChatService(void)
{
    m_msg_handler_map.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    m_msg_handler_map.insert({REG_MSG, std::bind(&ChatService::reg, this, \
    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
}

void ChatService::login(const muduo::net::TcpConnectionPtr &conn, \
    nlohmann::json &js, muduo::Timestamp time)
{
    LOG_INFO << "do login service!";
}
    // 处理注册业务
void ChatService::reg(const muduo::net::TcpConnectionPtr &conn, \
    nlohmann::json &js, muduo::Timestamp time)
{
    LOG_INFO << "do reg service!";
    std::string user_name = js["name"];
    std::string user_password = js["password"];

    User user;
    user.set_name(user_name);
    user.set_password(user_password);
    bool state = m_user_model.insert(user);
    if (state)
    {
        nlohmann::json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.get_id();
        conn->send(response.dump());
    }
    else
    {
        nlohmann::json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

MsgHandler ChatService::get_handler(int msgid)
{
    auto it = m_msg_handler_map.find(msgid);
    if (m_msg_handler_map.end() == it)
    {
        LOG_ERROR << "msgid" << msgid << " can not find handler";
    }

    return m_msg_handler_map[msgid];
}