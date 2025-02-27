#include "tcp_client_file_transfer.h"

// 构造函数，初始化m_clientfd为-1
ctcpclient::ctcpclient() : m_clientfd(-1) {}

// 向服务端发起连接请求，成功返回true，失败返回false
bool ctcpclient::connect(const std::string &in_ip, const unsigned short in_port) {
    // 如果socket已连接，直接返回失败
    if (m_clientfd != -1) return false;

    // 保存服务端的IP和端口到成员变量
    m_ip = in_ip;
    m_port = in_port;

    // 第1步：创建客户端的socket
    if ((m_clientfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) return false;

    // 第2步：向服务器发起连接请求
    struct sockaddr_in servaddr;               // 用于存放协议、端口和IP地址的结构体
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;             // ①协议族，固定填AF_INET
    servaddr.sin_port = htons(m_port);         // ②指定服务端的通信端口

    struct hostent* h;                         // 用于存放服务端IP地址(大端序)的结构体的指针
    // 把域名/主机名/字符串格式的IP转换成结构体
    if ((h = gethostbyname(m_ip.c_str())) == nullptr) {
        ::close(m_clientfd);
        m_clientfd = -1;
        return false;
    }
    // ③指定服务端的IP(大端序)
    memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);

    // 向服务端发起连接请求
    if (::connect(m_clientfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        ::close(m_clientfd);
        m_clientfd = -1;
        return false;
    }

    return true;
}

// 向服务端发送报文（字符串），成功返回true，失败返回false
bool ctcpclient::send(const std::string &buffer) {
    // 如果socket的状态是未连接，直接返回失败
    if (m_clientfd == -1) return false;

    // 发送字符串报文
    if ((::send(m_clientfd, buffer.data(), buffer.size(), 0)) <= 0) return false;

    return true;
}

// 向服务端发送报文（二进制数据），成功返回true，失败返回false
bool ctcpclient::send(void *buffer, const size_t size) {
    // 如果socket的状态是未连接，直接返回失败
    if (m_clientfd == -1) return false;

    // 发送二进制数据报文
    if ((::send(m_clientfd, buffer, size, 0)) <= 0) return false;

    return true;
}

// 接收服务端的报文，成功返回true，失败返回false
// buffer - 存放接收到的报文的内容，maxlen - 本次接收报文的最大长度
bool ctcpclient::recv(std::string &buffer, const size_t maxlen) {
    buffer.clear();         // 清空容器
    buffer.resize(maxlen);  // 设置容器的大小为maxlen
    // 直接操作buffer的内存接收数据
    int readn = ::recv(m_clientfd, &buffer[0], buffer.size(), 0);
    if (readn <= 0) {
        buffer.clear();
        return false;
    }
    buffer.resize(readn);   // 重置buffer的实际大小

    return true;
}

// 断开与服务端的连接
bool ctcpclient::close() {
    // 如果socket的状态是未连接，直接返回失败
    if (m_clientfd == -1) return false;

    ::close(m_clientfd);
    m_clientfd = -1;
    return true;
}

// 向服务端发送文件内容
bool ctcpclient::sendfile(const std::string &filename, const size_t filesize) {
    // 以二进制的方式打开文件
    std::ifstream fin(filename, std::ios::binary);
    if (fin.is_open() == false) {
        std::cout << "打开文件" << filename << "失败。\n";
        return false;
    }

    int onread = 0;        // 每次调用fin.read()时打算读取的字节数
    int totalbytes = 0;    // 从文件中已读取的字节总数
    char buffer[4096];       // 存放读取数据的buffer

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        // 计算本次应该读取的字节数，如果剩余的数据超过4096字节，就读4096字节
        if (filesize - totalbytes > 4096) onread = 4096;
        else onread = filesize - totalbytes;

        // 从文件中读取数据
        fin.read(buffer, onread);

        // 把读取到的数据发送给对端
        if (send(buffer, onread) == false) return false;

        // 计算文件已读取的字节总数，如果文件已读完，跳出循环
        totalbytes = totalbytes + onread;

        if (totalbytes == filesize) break;
    }

    return true;
}

// 析构函数
ctcpclient::~ctcpclient() {
    close();
}