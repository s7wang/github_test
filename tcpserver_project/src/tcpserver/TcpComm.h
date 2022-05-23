/**
 * @file TcpComm.h
 * @author wangs7
 * @brief A TCP Base Class. Inherit it to implement the local socket.
 * @version 0.1
 * @date 2022-01-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _TCPMANAGER_H__
#define _TCPMANAGER_H__

#include <memory>
#include <mutex>
#include <thread>
#include <pthread.h>
#include <set>
#include "tcputils/includes.h"
#include "tcputils/TcpSocket.h"
#include "tcputils/EpollUtils.h"
#include "tcputils/QueueManager.h"
/* num of thread */
#define THREAD_NUM              3       
/* thread id for array */
#define MAIN_TCP_THREAD         0       
#define SEND_TO_HANDLE_THREAD   1
#define SEND_TO_CLIENT_THREAD   2
/* other limited */
#define MAX_BUFFER_SIZE         2048
#define MAX_WRITE_COUNT         5
#define MAX_EVENT_SIZE          64

class TcpComm
{
private:
    /* TCP management resources */
    std::set<int> fd_set;
    std::mutex fd_set_mutex;

    std::unique_ptr<TcpSocket> tcp_socket_ptr;
    std::unique_ptr<EpollUtils> epoll_util_ptr;
    QueueManager *queue_manager_ptr;
    
    /* Thread management array */
    pthread_t thread_array[THREAD_NUM];

    mutable bool state_flag;
public:
    TcpComm();
    TcpComm(TcpComm &&) = default;
    TcpComm(const TcpComm &) = delete;
    TcpComm &operator=(TcpComm &&) = default;
    TcpComm &operator=(const TcpComm &) = delete;
    virtual ~TcpComm();
    
private:
    /**
     * @name _tcp_server_init
     * @brief 
     *      Internal mothed. Using init TCP socket, epoll
     * @param path local socket path
     * @return int 
     */
    int _tcp_server_init(const std::string &path);

    /**
     * @name _tcp_resources_release
     * @brief 
     *      Internal mothed. Release link and resources 
     */    
    void _tcp_resources_release();
    
    /**
     * @name _recv_from_client
     * @brief 
     *      Internal mothed. Receive data.
     * @param fd        src fd
     * @param buffer    buffer
     * @param max_size  buffer max size
     * @return int negative number=>error. else=>bytes read. 
     */   
    int _recv_from_client(int fd, Uint8* buffer, int max_size = MAX_BUFFER_SIZE);

    /**
     * @brief 
     *      send data to fd by socket.
     * @param fd    dest fd
     * @param data  data ptr
     * @param len   data len
     * @return int 
     */    
    int _send_to_one(int fd, const Uint8* data, int len);

    /**
     * @brief 
     *      send data to all fd by socket.
     * @param data  data ptr
     * @param len   data len
     * @return int 
     */
    int _send_to_all(const Uint8* data, int len);

public:
    /**
     * @brief 
     *      stsrt TCP server: 
     *          1.start send_to_handle_thread.
     *          2.start main_tcp_thread. init local socket and handle events.
     *          3.start send_to_client_thread.
     * @param path 
     */
    void tcp_server_start(const std::string &path);

    /**
     * @brief 
     *      stop threads and release source.
     */    
    void tcp_server_stop();

    /**
     * @brief 
     *      push data to send_queue.
     * @param fd    des fd
     * @param data  data ptr
     * @param len   data len
     * @return int 0-always success.
     */    
    int write_to_send_queue(int fd, Uint8* data, int len);

protected: /* The following three methods must be overridden */
    /**
     * @brief 
     *      virtual empty method.notify socket client add.
     *      if need know fd add,you should overwrite this method.
     * @param fd 
     */
    virtual void client_add_notify(int fd);

    /**
     * @brief 
     *      virtual empty method.notify socket client remove.
     *      if need know fd remove,you should overwrite this method.
     * @param fd 
     */    
    virtual void client_remove_notify(int fd);

    /**
     * @brief 
     *      handle data receive from socket. 
     *      this mothed will work in send_to_handle_thread.
     *      must be OVERWRITE!!!
     * @param fd 
     * @param data 
     * @param len 
     */    
    virtual void on_tcp_data_handle(int fd, Uint8* data, int len);

};



#endif // !_TCPMANAGER_H__