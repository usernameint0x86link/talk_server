#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

// 聊天客户端程序实现，main线程用作发送线程，子线程用作接收数据
int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "command invalid! example: ./ChatClient 127.0.0.1 8800" << std::endl;
        exit(-1);
    }

    const char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == client_fd)
    {
        std::cerr << "socket create error! " << std::endl;
        exit(-1);
    }

    sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    if (-1 == connect(client_fd, (struct sockaddr *)&server, sizeof(server)))
    {
        std::cerr << "connect to servererror! " << std::endl;
        close(client_fd);
        exit(-1);
    }

    return 0;
}