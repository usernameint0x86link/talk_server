#ifndef __CHATSERVER_H__
#define __CHATSERVER_H__

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

using namespace muduo;
using namespace muduo::net;

class ChatServer
{
public:
    // 初始化聊天服务器对象
    ChatServer(EventLoop *loop, \
        const InetAddress &listen_addr, \
        const string& name_arg);
    // 启动服务
    void start(void);


private:
    // 上报连接相关信息的回调函数
    void onConnection(const TcpConnectionPtr &);
    // 上报读写事件相关信息的回调函数 
    void onMessage(const TcpConnectionPtr &, Buffer *, Timestamp);
private:
    TcpServer m_server;
    EventLoop *m_loop;
};

#endif