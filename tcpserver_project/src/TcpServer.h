/**
 * @file TcpServer.h
 * @author wangs7
 * @brief test programe
 * @version 0.1
 * @date 2022-01-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _TCPSERVER_H__
#define _TCPSERVER_H__

#include "tcpserver/tcputils/includes.h"
#include "tcpserver/TcpComm.h"

class TcpServer : public TcpComm
{
public:
    TcpServer();
    TcpServer(TcpServer &&) = default;
    TcpServer(const TcpServer &) = delete;
    TcpServer &operator=(TcpServer &&) = default;
    TcpServer &operator=(const TcpServer &) = delete;
    ~TcpServer();
protected:
    /* OVERWRITE */
    virtual void client_add_notify(int fd);
    virtual void client_remove_notify(int fd);
    virtual void on_tcp_data_handle(int fd, Uint8* data, int len);
};



#endif // !_TCPSERVER_H__