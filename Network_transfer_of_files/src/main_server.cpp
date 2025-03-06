#include"tcp_server_file_transfer.h"

    // int m_listenfd;        // 监听的socket，-1表示未初始化
    // int m_clientfd;        // 客户端连上来的socket，-1表示客户端未连接
    // std::string m_clientip;     // 客户端字符串格式的IP
    // unsigned short m_port; // 服务端用于通讯的端口
int main(int argc, char *argv[]) {  
    if(argc!=3){
        std::cout<<"Using : ./main_server 端口 目录 \n";
        std::cout<<"Example: ./main_server 8080 /home/fcy/ \n";
        return 0;
    }
    for(int ii=1;ii<=64;ii++)
        signal(ii,SIG_IGN);//忽略信号,解决僵尸进程问题
    signal(SIGTERM,FathEXIT);//处理终止信号
    signal(SIGINT,FathEXIT);//处理中断信号
    
    if(tcpserver.initserver(atoi(argv[1]))==false){
        std::cout<<"初始化服务器失败！\n";
        return -1;
    }
    while(true){
        if(tcpserver.accept()==false){
            std::cout<<"接受客户端连接失败！\n";
            continue;
        }
      int pid=fork();
      if(pid==-1){
          std::cout<<"创建子进程失败！\n";
          return -1;
        }
      if(pid>0)    //父进程处理客户端请求
        {
            tcpserver.closeclient();
            continue;
        }
     
        tcpserver.closelisten();
        signal(SIGINT,SIG_IGN);
        signal(SIGTERM,ChldEXIT);
        struct st_fileinfo{
            char filename[100];
            int filesize;
        } fileinfo;
        memset(&fileinfo,0,sizeof(fileinfo));
        if(tcpserver.recv((void*)&fileinfo,sizeof(fileinfo))==false){
            std::cout<<"接收文件信息失败！\n";
            return -1;
        }
        if(tcpserver.send("ok") ==false){
            std::cout<<"发送确认信息失败！\n";
            return -1;
        }
        if(tcpserver.recvfile(std::string(argv[2])+"/"+std::string(fileinfo.filename),fileinfo.filesize)==false){
            std::cout<<"接收文件失败！\n";
            return -1;
        }
        if(tcpserver.send("ok") ==false){
            std::cout<<"发送确认信息失败！\n";
            return -1;
        }
        std::cout<<"文件传输成功！\n";
        return 0;
    }
    return 0;
}
    