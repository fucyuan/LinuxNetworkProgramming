#ifndef TCP_CLIENT_FILE_TRANSFER_H
#define TCP_CLIENT_FILE_TRANSFER_H

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

// TCP通讯的客户端类
class ctcpclient {
private:
    int m_clientfd;        // 客户端的socket，-1表示未连接或连接已断开；>=0表示有效的socket
    std::string m_ip;           // 服务端的IP/域名
    unsigned short m_port; // 通讯端口
public:
    // 构造函数，初始化m_clientfd为-1
    ctcpclient();

    // 向服务端发起连接请求，成功返回true，失败返回false
    bool connect(const std::string &in_ip, const unsigned short in_port);

    // 向服务端发送报文（字符串），成功返回true，失败返回false
    bool send(const std::string &buffer);

    // 向服务端发送报文（二进制数据），成功返回true，失败返回false
    bool send(void *buffer, const size_t size);

    // 接收服务端的报文，成功返回true，失败返回false
    // buffer - 存放接收到的报文的内容，maxlen - 本次接收报文的最大长度
    bool recv(std::string &buffer, const size_t maxlen);

    // 断开与服务端的连接
    bool close();

    // 向服务端发送文件内容
    bool sendfile(const std::string &filename, const size_t filesize);

    // 析构函数
    ~ctcpclient();
};

#endif // TCP_CLIENT_FILE_TRANSFER_H