/**
 * @file TcpSocket.h
 * @author wangs7
 * @brief Create local TCP socket and listen sfd
 * @version 0.1
 * @date 2021-12-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef _TCPSOCKET_H__
#define _TCPSOCKET_H__

#include "includes.h"

#include <string>
#include <string.h>
#include <mutex>

class TcpSocket {

private:
    std::string _local_socket_path; //local socket path
    int _server_socket_fd; //local socket fd
    std::mutex fd_mutex; //mutex of fd

public:
    TcpSocket(const std::string &local_socket_path);
    
    TcpSocket(const TcpSocket &) = delete;
    TcpSocket(TcpSocket &&) = default;
    TcpSocket &operator=(const TcpSocket &) = delete;
    TcpSocket &operator=(TcpSocket &&) = default;
    virtual ~TcpSocket();
    
    /**
     * @name tcp_socket_init
     * @brief Create local socket and listen sfd
     * 
     */
    void tcp_socket_init();

    /**
     * @name get_server_sfd
     * @brief get server socket fd.
     * 
     * @return int sfd
     */
    int get_server_sfd();

    void set_path(const std::string &path);


};


#endif // !_TCPSOCKET_H__