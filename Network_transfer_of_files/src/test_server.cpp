#include"tcp_server_file_transfer.h"

    // int m_listenfd;        // 监听的socket，-1表示未初始化
    // int m_clientfd;        // 客户端连上来的socket，-1表示客户端未连接
    // std::string m_clientip;     // 客户端字符串格式的IP
    // unsigned short m_port; // 服务端用于通讯的端口
ctcpserver::ctcpserver():m_listenfd(-1),m_clientfd(-1),m_clientip(""),m_port(0){
}