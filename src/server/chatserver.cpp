#include "chatserver.h"
#include "chatservice.h"
#include "json.hpp"

#include <functional>
#include <string>
#include <iostream>

using namespace std;
using namespace placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop, const InetAddress &listen_addr, const string& name_arg) : m_server(loop, listen_addr, name_arg), m_loop(loop)
{
    // 注册链接回调
    m_server.setConnectionCallback(\
    std::bind(&ChatServer::onConnection, this, \
    std::placeholders::_1));
    //注册消息回调
    m_server.setMessageCallback(std::bind(&ChatServer::onMessage, this, \
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    // 设置线程数量
    m_server.setThreadNum(4);
}

void ChatServer::start(void)
{ m_server.start(); }

// 上报连接相关信息的回调函数
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    // user shutdown link
    if (!conn->connected())
    { 
        ChatService::instance()->client_close_exception(conn);
        conn->shutdown();
    }
    // std::cout << "连接上了" << std::endl;
}
// 上报读写事件相关信息的回调函数 
void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
{
    // std::cout << "haneler" << std::endl;
    std::string buf = buffer->retrieveAllAsString();

    std::cout << buf << std::endl;

    json js = json::parse(buf);

    auto msg_handler = ChatService::instance()->get_handler(js["msgid"].get<int>());
    msg_handler(conn, js, time);
}