# Network File Transfer Project README

## 项目概述
本项目实现了一个基于TCP协议的文件传输系统，包含客户端和服务端两个部分。客户端负责将文件传输至服务端，服务端则接收并存储文件。该项目旨在展示如何在Linux环境下利用C++进行网络编程实现可靠的文件传输功能。

## 项目结构
```
.
├── build
├── include
│   ├── tcp_client_file_transfer.h
│   └── tcp_server_file_transfer.h
├── src
│   ├── tcp_client_file_transfer_impl.cpp
│   ├── tcp_client_file_transfer_main.cpp
│   ├── tcp_server_file_transfer_impl.cpp
│   └── tcp_server_file_transfer_main.cpp
├── CMakeLists.txt
└── README.md
```
- **build**：存放编译生成的文件，包括可执行文件以及编译过程中产生的中间文件。
- **include**：包含项目所需的头文件。`tcp_client_file_transfer.h`定义了客户端类的接口，`tcp_server_file_transfer.h`定义了服务端类的接口。
- **src**：存放项目的源文件。`tcp_client_file_transfer_impl.cpp`实现了客户端类的功能，`tcp_client_file_transfer_main.cpp`是客户端的主程序入口；`tcp_server_file_transfer_impl.cpp`实现了服务端类的功能，`tcp_server_file_transfer_main.cpp`是服务端的主程序入口。
- **CMakeLists.txt**：用于项目构建的CMake脚本，定义了项目的名称、使用的C++标准、包含目录、可执行文件的生成以及链接库的设置等。
- **README.md**：本说明文档。

## 编译与运行
### 编译
1. **确保环境准备**：
    - 系统安装有CMake（版本3.10及以上）。
    - 安装有GCC编译器（用于编译C++代码）。
2. **编译步骤**：
    - 打开终端，进入项目根目录。
    - 创建并进入`build`目录：
```sh
mkdir build
cd build
```
    - 运行CMake生成构建文件：
```sh
cmake..
```
    - 执行`make`命令进行编译：
```sh
make
```
编译成功后，在`build`目录下会生成`tcp_client_file_transfer`和`tcp_server_file_transfer`两个可执行文件。

### 运行
1. **服务端运行**：
在终端中，进入`build`目录，运行服务端程序，命令格式如下：
```sh
./tcp_server_file_transfer [通讯端口] [文件存放的目录]
```
例如：
```sh
./tcp_server_file_transfer 5005 /tmp
```
服务端启动后，会监听指定端口，等待客户端连接。

2. **客户端运行**：
在另一个终端中，进入`build`目录，运行客户端程序，命令格式如下：
```sh
./tcp_client_file_transfer [服务端的IP] [服务端的端口] [文件名] [文件大小]
```
例如：
```sh
./tcp_client_file_transfer 192.168.1.100 5005 aaa.txt 2424
```
客户端会连接到指定IP和端口的服务端，并将指定文件传输至服务端。

## 注意事项
1. **防火墙设置**：运行服务端程序的Linux系统的防火墙必须开通服务端监听的端口（如示例中的5005端口）。如果是在云服务器上运行，还需要在云平台的安全组或访问策略中开通相应端口。
2. **参数准确性**：运行客户端和服务端程序时，请确保输入的参数准确无误。特别是客户端的服务端IP、端口以及文件相关参数，服务端的端口和文件存放目录参数。
3. **文件路径**：服务端的文件存放目录必须存在且有写入权限，否则文件接收可能失败。
