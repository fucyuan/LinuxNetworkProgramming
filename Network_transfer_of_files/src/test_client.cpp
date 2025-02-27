#include "tcp_client_file_transfer.h"



    // int m_clientfd;        // 客户端的socket，-1表示未连接或连接已断开；>=0表示有效的socket
    // std::string m_ip;           // 服务端的IP/域名
    // unsigned short m_port; // 通讯端口

ctcpclient::ctcpclient() : m_clientfd(-1){

}


bool ctcpclient::connect(const std::string &in_ip, const unsigned short in_port) {
    if(m_clientfd!=-1)return false;
    m_ip = in_ip;
    m_port = in_port;
    /*第一步分：创建socket*/
    m_clientfd=socket(AF_INET,SOCK_STREAM,0);
    /*
    socket的第一个参数指定协议族，这里是IPv4，所以是AF_INET；
    第二个参数指定socket类型，这里是TCP流式socket，所以是SOCK_STREAM；
    第三个参数指定协议，这里是默认的，所以是0。还可以指定其他协议，如SOCK_DGRAM表示UDP数据报socket。
    */
   //第二部分：连接服务器
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));//将server_addr清零
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(m_port);
    struct hostent* h;
    /*
    hostent结构体包含了主机名、IP地址、主机别名、主机地址列表等信息。
    */
    if((h=gethostbyname(m_ip.c_str()))==nullptr) {
        std::cerr<<"gethostbyname error"<<std::endl;
        ::close(m_clientfd);
        m_clientfd=-1;
        return false;
    }
    memcpy(&server_addr.sin_addr,h->h_addr,h->h_length);
    /*
    第三步：连接服务器
    调用connect函数，将客户端的socket和服务器的地址联系起来。
    第一个参数是客户端的socket；
    第二个参数是指向服务器地址的指针，这里是一个sockaddr_in结构体，包含了服务器的IP地址和端口号。
    第三个参数是地址结构的长度，这里是sizeof(sockaddr_in)。
    */
    if(::connect(m_clientfd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0) {
        std::cerr<<"connect error"<<std::endl;
        ::close(m_clientfd);
        m_clientfd=-1;
        return false;
    }
    return true;
}

bool ctcpclient::send(const std::string &buffer) {
    if(m_clientfd<0)return false;
    if(::send(m_clientfd,buffer.c_str(),buffer.size(),0)<0) {
        std::cerr<<"send error"<<std::endl;
        ::close(m_clientfd);
        m_clientfd=-1;
        return false;
    }
    return true;
}

bool ctcpclient::send(void *buffer, const size_t size) {
    if(m_clientfd<0)return false;
    if(::send(m_clientfd,buffer,size,0)<0) {
        std::cerr<<"send error"<<std::endl;
        ::close(m_clientfd);
        m_clientfd=-1;
        return false;
    }
    return true;
}

bool ctcpclient::recv(std::string &buffer, const size_t size) {
    if(m_clientfd<0)return false;
    buffer.clear();
    buffer.resize(size);
    if(::recv(m_clientfd,&buffer[0],size,0)<0) {
        std::cerr<<"recv error"<<std::endl;
        ::close(m_clientfd);
        m_clientfd=-1;
        return false;
    }
    buffer.resize(size-1);
    return true;
}

bool ctcpclient::close() {
    if(m_clientfd<0)return false;
    ::close(m_clientfd);
    m_clientfd=-1;
    return true;
}

bool ctcpclient::sendfile(const std::string &filename, const size_t size) {
    if(m_clientfd<0)return false;
    std::ifstream fin(filename.c_str(),std::ios::binary);
    /*
    两个参数：第一个是文件名，第二个是打开方式，这里是以二进制方式打开。
    */
    if(!fin) {
        std::cerr<<"open file error"<<std::endl;
        return false;
    }
    char buffer[1024];
    size_t len=0;
    while(len<size) {
        fin.read(buffer,1024);
        len+=fin.gcount();
        if(len>size)len=size;
        if(!send(buffer,len)) {
            std::cerr<<"send file error"<<std::endl;
            fin.close();
            return false;
        }
    }
    fin.close();
    return true;
}

ctcpclient::~ctcpclient() {
    if(m_clientfd>=0)close();
}

int main(int argc, char *argv[]) { 
    if(argc!=5) {
        std::cerr<<"Usage: "<<argv[0]<<" ip port filename filesize"<<std::endl;
        std::cout<<"Example: "<<argv[0]<<" 127.0.0.1 8080 test.txt 1024"<<std::endl;
        return 1;
    }
    ctcpclient tcpclient;
    if(tcpclient.connect(argv[1],atoi(argv[2]))==false) {
        std::cerr<<"connect error"<<std::endl;
        return 1;
    }
    struct st_fileinfo {
        char filename[256];
        size_t filesize;
    } fileinfo;
    memset(&fileinfo,0,sizeof(fileinfo));
    strcpy(fileinfo.filename,argv[3]);
    fileinfo.filesize=atoi(argv[4]);//将文件大小转化为size_t类型\
    //第一步：发送文件信息
    if(tcpclient.send((void*)&fileinfo,sizeof(fileinfo))==false) {
        std::cerr<<"send fileinfo error"<<std::endl;
        perror("send");//perror函数用来打印出系统错误信息
        return -1;
    }
    //第二步：接受服务器的响应
    std::string buffer;
    if(tcpclient.recv(buffer,2)==false) {
        std::cerr<<"recv error"<<std::endl;
        return -1;
    }
    if(buffer!="ok") {
        std::cerr<<"recv error"<<std::endl;
        return -1;
    }
    //第三步：发送文件内容
    if(tcpclient.sendfile(fileinfo.filename,fileinfo.filesize)==false) {
        std::cerr<<"send file error"<<std::endl;
        return -1;
    }
    //第四步：接受服务器的响应
    if(tcpclient.recv(buffer,2)==false) {
        std::cerr<<"recv error"<<std::endl;
        return -1;
    }
    if(buffer!="ok") {
        std::cerr<<"recv error"<<std::endl;
        return -1;
    }
    std::cout<<"send file success"<<std::endl;
    return 0;
}   

 