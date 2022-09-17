#include "chatserver.h"
#include "db.h"

#include <iostream>

int main(void)
{
    muduo::net::EventLoop loop;
    muduo::net::InetAddress addr("127.0.0.1", 8800);
    std::string server_name("chat_server");
    ChatServer sever(&loop, addr, server_name);

    sever.start();
    loop.loop();

    return 0;
}