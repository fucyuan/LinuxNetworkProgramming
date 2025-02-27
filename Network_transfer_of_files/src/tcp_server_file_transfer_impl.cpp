#include "tcp_server_file_transfer.h"

// 构造函数，初始化m_listenfd和m_clientfd为-1
ctcpserver::ctcpserver() : m_listenfd(-1), m_clientfd(-1) {}

// 初始化服务端用于监听的socket
bool ctcpserver::initserver(const unsigned short in_port) {
    // 第1步：创建服务端的socket
    if ((m_listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) return false; 

    m_port = in_port;

    // 第2步：把服务端用于通信的IP和端口绑定到socket上
    struct sockaddr_in servaddr;                // 用于存放协议、端口和IP地址的结构体
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;                // ①协议族，固定填AF_INET
    servaddr.sin_port = htons(m_port);            // ②指定服务端的通信端口
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // ③如果操作系统有多个IP，全部的IP都可以用于通讯

    // 绑定服务端的IP和端口（为socket分配IP和端口）
    if (bind(m_listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) { 
        close(m_listenfd); 
        m_listenfd = -1; 
        return false; 
    }

    // 第3步：把socket设置为可连接（监听）的状态
    if (listen(m_listenfd, 5) == -1) { 
        close(m_listenfd); 
        m_listenfd = -1; 
        return false;
    }

    return true;
}

// 受理客户端的连接（从已连接的客户端中取出一个客户端）
// 如果没有已连接的客户端，accept()函数将阻塞等待
bool ctcpserver::accept() {
    struct sockaddr_in caddr;        // 客户端的地址信息
    socklen_t addrlen = sizeof(caddr); // struct sockaddr_in的大小
    if ((m_clientfd = ::accept(m_listenfd, (struct sockaddr *)&caddr, &addrlen)) == -1) return false;

    m_clientip = inet_ntoa(caddr.sin_addr);  // 把客户端的地址从大端序转换成字符串

    return true;
}

// 获取客户端的IP(字符串格式)
const std::string & ctcpserver::clientip() const {
    return m_clientip;
}

// 向对端发送报文，成功返回true，失败返回false
bool ctcpserver::send(const std::string &buffer) { 
    if (m_clientfd == -1) return false;

    if ((::send(m_clientfd, buffer.data(), buffer.size(), 0)) <= 0) return false;

    return true;
}

// 接收对端的报文（字符串），成功返回true，失败返回false
// buffer - 存放接收到的报文的内容，maxlen - 本次接收报文的最大长度
bool ctcpserver::recv(std::string &buffer, const size_t maxlen) { 
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

// 接收客户端的报文（二进制数据），成功返回true，失败返回false
// buffer - 存放接收到的报文的内容，size - 本次接收报文的最大长度
bool ctcpserver::recv(void *buffer, const size_t size) {
    if (::recv(m_clientfd, buffer, size, 0) <= 0) return false;

    return true;
}

// 关闭监听的socket
bool ctcpserver::closelisten() {
    if (m_listenfd == -1) return false; 

    ::close(m_listenfd);
    m_listenfd = -1;
    return true;
}

// 关闭客户端连上来的socket
bool ctcpserver::closeclient() {
    if (m_clientfd == -1) return false;

    ::close(m_clientfd);
    m_clientfd = -1;
    return true;
}

// 接收文件内容
bool ctcpserver::recvfile(const std::string &filename, const size_t filesize) {
    std::ofstream fout;
    fout.open(filename, std::ios::binary);
    if (fout.is_open() == false) { 
        std::cout << "打开文件" << filename << "失败。\n";  
        return false;
    }

    int totalbytes = 0;        // 已接收文件的总字节数
    int onread = 0;            // 本次打算接收的字节数
    char buffer[4096];         // 接收文件内容的缓冲区

    while (true) {
        // 计算本次应该接收的字节数
        if (filesize - totalbytes > 4096) onread = 4096; 
        else onread = filesize - totalbytes; 

        // 接收文件内容
        if (recv(buffer, onread) == false) return false; 

        // 把接收到的内容写入文件
        fout.write(buffer, onread); 

        // 计算已接收文件的总字节数，如果文件接收完，跳出循环
        totalbytes = totalbytes + onread; 

        if (totalbytes == filesize) break;
    }

    return true;
}

ctcpserver::~ctcpserver() { 
    closelisten(); 
    closeclient(); 
}

ctcpserver tcpserver;

// 父进程的信号处理函数
void FathEXIT(int sig) {
    // 以下代码是为了防止信号处理函数在执行的过程中再次被信号中断
    signal(SIGINT, SIG_IGN); 
    signal(SIGTERM, SIG_IGN);

    std::cout << "父进程退出，sig = " << sig << std::endl;

    kill(0, SIGTERM);     // 向全部的子进程发送15的信号，通知它们退出

    // 在这里增加释放资源的代码（全局的资源）
    tcpserver.closelisten();       // 父进程关闭监听的socket

    exit(0);
}

// 子进程的信号处理函数
void ChldEXIT(int sig) {
    // 以下代码是为了防止信号处理函数在执行的过程中再次被信号中断
    signal(SIGINT, SIG_IGN); 
    signal(SIGTERM, SIG_IGN);

    std::cout << "子进程" << getpid() << "退出，sig = " << sig << std::endl;

    // 在这里增加释放资源的代码（只释放子进程的资源）
    tcpserver.closeclient();       // 子进程关闭客户端连上来的socket

    exit(0);
}