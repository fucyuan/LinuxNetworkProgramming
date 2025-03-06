#include<tcp_client_file_transfer.h>

int main(int argc,char *argv[]){
    if(argc!=5){//参数个数必须为5
        std::cout<<"Using :./client 服务ip 端口 文件名 文件大小"<<std::endl;
        std::cout<<"Example:./client 127.0.0.1 8080 test.txt 1024"<<std::endl;
        return -1;
    }
    ctcpclient tcpclient;
    //第一步：连接服务器
    if(tcpclient.connect(argv[1],atoi(argv[2]))==false){
        std::cout<<"Connect failed"<<std::endl;
        return -1;
    }
    //第二步：发送文件名和大小
    struct st_fileinfo{
        char filename[100];
        int filesize;
    }fileinfo;
    memset(&fileinfo,0,sizeof(fileinfo));
    strcpy(fileinfo.filename,argv[3]);
    fileinfo.filesize=atoi(argv[4]);
    if(tcpclient.send((void*)&fileinfo,sizeof(fileinfo))==false){
        std::cout<<"Send fileinfo failed"<<std::endl;
        return -1;
    }
    //第三步：接收服务器响应
    std::string buffer;
    if(tcpclient.recv(buffer,2)==false){
        std::cout<<"Recv response failed"<<std::endl;
        return -1;
    }
    if(buffer!="ok"){
        std::cout<<"Recv response error"<<std::endl;
        return -1;
    }
    //第四步：发送文件内容
    if(tcpclient.sendfile(fileinfo.filename,fileinfo.filesize)==false){
        std::cout<<"Send file content failed"<<std::endl;
        return -1;
    }
    //第五步：接收服务器响应
    if(tcpclient.recv(buffer,2)==false){
        std::cout<<"Recv response failed"<<std::endl;
        return -1;
    }
    if(buffer!="ok"){
        std::cout<<"Recv response error"<<std::endl;
        return -1;
    }
    std::cout<<"Send file success"<<std::endl;
    return 0;
}