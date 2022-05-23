/**
 * @file TcpSocket.cpp
 * @author wangs7
 * @brief 
 * @version 0.1
 * @date 2021-12-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "TcpSocket.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>

TcpSocket::TcpSocket(const std::string &local_socket_path):_local_socket_path(local_socket_path)
{
    _server_socket_fd = -1;
}

TcpSocket::~TcpSocket()
{

}
/**
 * @name tcp_socket_init
 * @brief Create local socket and listen sfd
 * 
 */
void TcpSocket::tcp_socket_init()
{
    /* Create local TCP socket */
    std::lock_guard<std::mutex> lock(fd_mutex); //locked fd
    _server_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0/* IPPROTO_TCP IPPROTO_SCTP*/);
    if (_server_socket_fd < 0) 
    {
        fprintf(stderr, "[%s]:socket error:%s.\n", __func__, strerror(errno));
        return; //unlock mutex
    }
    const char* path_name = _local_socket_path.c_str();
    if (!access(path_name, F_OK))
    {
        fprintf(stdout, "path:%s file already exists and remove old file.\n", path_name);
        remove(path_name);
    }

    /* local UNIX domain socket address. */
    struct sockaddr_un local_socket_addr;
    socklen_t addrlen = sizeof(local_socket_addr);
    bzero(&local_socket_addr, addrlen);
    local_socket_addr.sun_family = AF_UNIX;
    strncpy(local_socket_addr.sun_path, path_name, sizeof(local_socket_addr.sun_path));

    /* bind local socket. */
    if (bind(_server_socket_fd, (struct sockaddr *)&local_socket_addr, addrlen) < 0)
    {
        fprintf(stderr, "[%s]:bind error:%s.\n", __func__, strerror(errno));
        unlink(path_name);
        return; //unlock mutex
    }

    /* listen local socket. */
    if (listen(_server_socket_fd, MAX_LISTEN_QUEUE_SIZE) < 0)
    {
        fprintf(stderr, "[%s]:listen error:%s.\n", __func__, strerror(errno));
        return; //unlock mutex
    }
} //unlock mutex

/**
 * @name get_server_sfd
 * @brief get server socket fd.
 * 
 * @return int sfd
 */
int TcpSocket::get_server_sfd()
{
    std::lock_guard<std::mutex> lock(fd_mutex); //locked fd
    return _server_socket_fd; //unlock mutex
} 

void TcpSocket::set_path(const std::string &path)
{
    _local_socket_path = path;
}