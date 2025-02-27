#include "tcp_client_file_transfer.h"

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cout << "Using:./tcp_client_file_transfer 服务端的IP 服务端的端口 文件名 文件大小\n";
        std::cout << "Example:./tcp_client_file_transfer 192.168.101.138 5005 aaa.txt 2424\n\n";
        return -1;
    }

    ctcpclient tcpclient;
    // 向服务端发起连接请求
    if (tcpclient.connect(argv[1], std::atoi(argv[2])) == false) {
        perror("connect()");
        return -1;
    }

    // 以下是发送文件的流程
    // 1）把待传输文件名和文件的大小告诉服务端
    // 定义文件信息的结构体
    struct st_fileinfo {
        char filename[256];  // 文件名
        int filesize;        // 文件大小
    } fileinfo;
    memset(&fileinfo, 0, sizeof(fileinfo));
    strcpy(fileinfo.filename, argv[3]);     // 文件名
    fileinfo.filesize = std::atoi(argv[4]);       // 文件大小
    // 把文件信息的结构体发送给服务端
    if (tcpclient.send(&fileinfo, sizeof(fileinfo)) == false) {
        perror("send");
        return -1;
    }
    std::cout << "发送文件信息的结构体" << fileinfo.filename << "(" << fileinfo.filesize << ")。" << std::endl;

    // 2）等待服务端的确认报文（文件名和文件的大小的确认）
    std::string buffer;
    if (tcpclient.recv(buffer, 2) == false) {
        perror("recv()");
        return -1;
    }
    if (buffer != "ok") {
        std::cout << "服务端没有回复ok。\n";
        return -1;
    }

    // 3）发送文件内容
    if (tcpclient.sendfile(fileinfo.filename, fileinfo.filesize) == false) {
        perror("sendfile()");
        return -1;
    }

    // 4）等待服务端的确认报文（服务端已接收完文件）
    if (tcpclient.recv(buffer, 2) == false) {
        perror("recv()");
        return -1;
    }
    if (buffer != "ok") {
        std::cout << "发送文件内容失败。\n";
        return -1;
    }

    std::cout << "发送文件内容成功。\n";

    return 0;
}