#include"tcp_server_file_transfer.h"
/*
TCP服务器端流程：socket() → bind() → listen() → accept() → send()/recv()

*/
    // int m_listenfd;        // 监听的socket，-1表示未初始化
    // int m_clientfd;        // 客户端连上来的socket，-1表示客户端未连接
    // std::string m_clientip;     // 客户端字符串格式的IP
    // unsigned short m_port; // 服务端用于通讯的端口
ctcpserver::ctcpserver():m_listenfd(-1),m_clientfd(-1),m_clientip(""),m_port(0){
} 
bool ctcpserver::initserver(const unsigned short port){
    // 1.创建socket
   if ((m_listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) return false; 
   m_port = port;
   //2.绑定IP和端口
   struct sockaddr_in server_addr;
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   server_addr.sin_port = htons(m_port);
   if (bind(m_listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) return false;
   //3.监听
   if (listen(m_listenfd, 10) == -1) return false;
   return true;
}
bool ctcpserver::accept() {
    struct sockaddr_in caddr;        
    socklen_t addrlen = sizeof(caddr); 
    if ((m_clientfd = ::accept(m_listenfd, (struct sockaddr *)&caddr, &addrlen)) == -1) return false;
    m_clientip = inet_ntoa(caddr.sin_addr);  
    return true;
}
const std::string& ctcpserver::clientip() const {
    return m_clientip;
}

bool ctcpserver::send(const std::string& buffer) {
    if (m_clientfd == -1) return false;
    int n = ::send(m_clientfd, buffer.c_str(), buffer.size(), 0);
    if (n == -1) return false;
    return true;
}
bool ctcpserver::recv(std::string& buffer,const size_t len)  
{
    if (m_clientfd == -1) return false;
    buffer.clear();
    buffer.resize(len);
    int n = ::recv(m_clientfd, &buffer[0], len, 0);
    if (n == -1) return false;
    buffer.resize(n);
    return true;
}

bool ctcpserver::closelisten() {
    if (m_listenfd != -1) {
        ::close(m_listenfd);
        m_listenfd = -1;
    }
    return true;
}

bool ctcpserver::closeclient() {
    if (m_clientfd != -1) {
        ::close(m_clientfd);
        m_clientfd = -1;
    }
    return true;
}   

void FathEXIT(int sig) {
    std::cout << "Father process exit." << std::endl;
    signal(SIGTERM, SIG_IGN);
    signal(SIGINT, SIG_IGN);

    exit(0);
}

