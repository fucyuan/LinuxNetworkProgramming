#ifndef TCP_SERVER_FILE_TRANSFER_H
#define TCP_SERVER_FILE_TRANSFER_H

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

// TCP通讯的服务端类
class ctcpserver {
private:
    int m_listenfd;        // 监听的socket，-1表示未初始化
    int m_clientfd;        // 客户端连上来的socket，-1表示客户端未连接
    std::string m_clientip;     // 客户端字符串格式的IP
    unsigned short m_port; // 服务端用于通讯的端口
public:
    // 构造函数，初始化m_listenfd和m_clientfd为-1
    ctcpserver();

    // 初始化服务端用于监听的socket
    bool initserver(const unsigned short in_port);

    // 受理客户端的连接（从已连接的客户端中取出一个客户端）
    // 如果没有已连接的客户端，accept()函数将阻塞等待
    bool accept();

    // 获取客户端的IP(字符串格式)
    const std::string & clientip() const;

    // 向对端发送报文，成功返回true，失败返回false
    bool send(const std::string &buffer);

    // 接收对端的报文（字符串），成功返回true，失败返回false
    // buffer - 存放接收到的报文的内容，maxlen - 本次接收报文的最大长度
    bool recv(std::string &buffer, const size_t maxlen);

    // 接收客户端的报文（二进制数据），成功返回true，失败返回false
    // buffer - 存放接收到的报文的内容，size - 本次接收报文的最大长度
    bool recv(void *buffer, const size_t size);

    // 关闭监听的socket
    bool closelisten();

    // 关闭客户端连上来的socket
    bool closeclient();

    // 接收文件内容
    bool recvfile(const std::string &filename, const size_t filesize);

    ~ctcpserver();
};

extern ctcpserver tcpserver;

void FathEXIT(int sig);  // 父进程的信号处理函数
void ChldEXIT(int sig);  // 子进程的信号处理函数

#endif // TCP_SERVER_FILE_TRANSFER_H