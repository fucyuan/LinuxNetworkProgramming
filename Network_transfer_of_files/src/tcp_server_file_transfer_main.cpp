#include "tcp_server_file_transfer.h"

int main(int argc, char *argv[]) {
    if (argc!= 3) {
        std::cout << "Using:./tcp_server_file_transfer 通讯端口 文件存放的目录\n";
        std::cout << "Example:./tcp_server_file_transfer 5005 /tmp\n\n";
        std::cout << "注意：运行服务端程序的Linux系统的防火墙必须要开通5005端口。\n";
        std::cout << "      如果是云服务器，还要开通云平台的访问策略。\n\n";
        return -1;
    }

    // 忽略全部的信号，不希望被打扰。顺便解决了僵尸进程的问题
    for (int ii = 1; ii <= 64; ii++) signal(ii, SIG_IGN);

    // 设置信号,在shell状态下可用 "kill 进程号" 或 "Ctrl+c" 正常终止些进程
    // 但请不要用 "kill -9 +进程号" 强行终止
    signal(SIGTERM, FathEXIT); 
    signal(SIGINT, FathEXIT);  // SIGTERM 15 SIGINT 2

    // 初始化服务端用于监听的socket
    if (tcpserver.initserver(atoi(argv[1])) == false) { 
        perror("initserver()"); 
        return -1;
    }

    while (true) {
        // 受理客户端的连接（从已连接的客户端中取出一个客户端）
        // 如果没有已连接的客户端，accept()函数将阻塞等待
        if (tcpserver.accept() == false) {
            perror("accept()"); 
            return -1;
        }

        int pid = fork();
        if (pid == -1) { 
            perror("fork()"); 
            return -1; 
        }  // 系统资源不足
        if (pid > 0) { 
            // 父进程
            tcpserver.closeclient();  // 父进程关闭客户端连接的socket
            continue;

         // 父进程返回到循环开始的位置，继续受理客户端的连接
        }

        tcpserver.closelisten();    // 子进程关闭监听的socket

        // 子进程需要重新设置信号
        signal(SIGTERM, ChldEXIT);   // 子进程的退出函数与父进程不一样
        signal(SIGINT, SIG_IGN);     // 子进程不需要捕获SIGINT信号

        // 子进程负责与客户端进行通讯
        std::cout << "客户端已连接(" << tcpserver.clientip() << ")。\n";

        // 以下是接收文件的流程
        // 1）接收文件名和文件大小信息
        // 定义文件信息的结构体
        struct st_fileinfo {
            char filename[256];  // 文件名
            int filesize;        // 文件大小
        } fileinfo;
        memset(&fileinfo, 0, sizeof(fileinfo));
        // 用结构体存放接收报文的内容
        if (tcpserver.recv(&fileinfo, sizeof(fileinfo)) == false) { 
            perror("recv()"); 
            return -1;
        }
        std::cout << "文件信息结构体" << fileinfo.filename << "(" << fileinfo.filesize << ")。" << std::endl;

        // 2）给客户端回复确认报文，表示客户端可以发送文件了
        if (tcpserver.send("ok") == false) { 
            perror("send"); 
            break;
        }

        // 3）接收文件内容
        if (tcpserver.recvfile(std::string(argv[2]) + "/" + fileinfo.filename, fileinfo.filesize) == false) { 
            std::cout << "接收文件内容失败。\n"; 
            return -1; 
        }

        std::cout << "接收文件内容成功。\n";

        // 4）给客户端回复确认报文，表示文件已接收成功
        tcpserver.send("ok");

        return 0;  // 子进程一定要退出，否则又会回到accept()函数的位置
    }
}