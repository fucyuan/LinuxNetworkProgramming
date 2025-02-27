# CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(NetworkFileTransfer)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)

# 包含头文件目录
include_directories(include)

# 定义可执行文件
add_executable(tcp_client_file_transfer
    src/tcp_client_file_transfer_impl.cpp
    src/tcp_client_file_transfer_main.cpp
)

# 定义可执行文件
add_executable(tcp_server_file_transfer
    src/tcp_server_file_transfer_impl.cpp
    src/tcp_server_file_transfer_main.cpp
)

# 为可执行文件添加链接库
# 对于网络编程，通常需要链接 socket 相关库，在 Linux 下一般是 -lstdc++ 和 -lpthread
target_link_libraries(tcp_client_file_transfer stdc++ pthread)
target_link_libraries(tcp_server_file_transfer stdc++ pthread)