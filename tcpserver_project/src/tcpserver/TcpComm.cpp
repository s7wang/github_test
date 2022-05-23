/**
 * @file TcpComm.cpp
 * @author ywangs7
 * @brief 
 * @version 0.1
 * @date 2022-01-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "TcpComm.h"

#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>

TcpComm::TcpComm()
{
    state_flag = false;
}

TcpComm::~TcpComm()
{
}

/**
 * @name _tcp_server_init
 * @brief 
 *      Internal mothed. Using init TCP socket, epoll
 * @param path local socket path
 * @return int 
 */
int TcpComm::_tcp_server_init(const std::string &path)
{
    tcp_socket_ptr = std::unique_ptr<TcpSocket> (new TcpSocket(path));
    epoll_util_ptr = std::unique_ptr<EpollUtils> (new EpollUtils());
    queue_manager_ptr = QueueManager::get_instance();

    if (tcp_socket_ptr.get() == nullptr) 
    {
        fprintf(stderr, "[%s]:tcp_socket_ptr is null.\n", __func__);
        fprintf(stdout, "========[%s]:TCP init failed!========\n", __func__);
        return -1;
    }
    tcp_socket_ptr.get()->tcp_socket_init();
    const int fd = tcp_socket_ptr.get()->get_server_sfd();
    fprintf(stdout, "TCP server start.\n");
    fprintf(stdout, "sfd = %d. Add server fd to epoll:\n", fd);
    epoll_util_ptr.get()->epoll_init();
    epoll_util_ptr.get()->epoll_add(fd);
    fprintf(stdout, "========[%s]:TCP init successed!========\n", __func__);
    return 0;
}


/**
 * @name _tcp_resources_release
 * @brief 
 *      Internal mothed. Release link and resources 
 */
void TcpComm::_tcp_resources_release()
{
    tcp_socket_ptr.reset();
    epoll_util_ptr.reset();
    
    if (queue_manager_ptr != nullptr)
    {
        queue_manager_ptr->clear();
    }
    std::lock_guard<std::mutex> lock(fd_set_mutex);
    fd_set.clear();
    fprintf(stdout, "========[%s]:TCP resources released!========\n", __func__);
}

/**
 * @name _recv_from_client
 * @brief 
 *      Internal mothed. Receive data.
 * @param fd        src fd
 * @param buffer    buffer
 * @param max_size  buffer max size
 * @return int negative number=>error. else=>bytes read. 
 */
int TcpComm::_recv_from_client(int fd, Uint8* buffer, int max_size)
{
    if (fd <= 2 || buffer == nullptr)
    {
        fprintf(stderr, "[%s]:fd = %d\t buffer = %d\n", __func__, fd, buffer);
        return -1;
    }
    return read(fd, buffer, max_size);
}

/**
 * @brief 
 *      send data to fd by socket.
 * @param fd    dest fd
 * @param data  data ptr
 * @param len   data len
 * @return int 
 */
int TcpComm::_send_to_one(int fd, const Uint8* data, int len)
{
    if (data == NULL || len <= 0)
    {
        fprintf(stderr, "data is NULL or len <= 0 !\n");
        return 0;
    }
    int count = 0;
    int size = 0;
    std::lock_guard<std::mutex> lock(fd_set_mutex);
    if (fd_set.find(fd) == fd_set.end())
    {
        fprintf(stderr, "fd = %d not exist!\n", fd);
    }
    while ((len - size) != 0)
    {
        int ret = write(fd, data + size, len - size);
        if (ret < 0)
        {
            if (errno == EAGAIN)
            {
                if ( ++count > MAX_WRITE_COUNT)
                {
                    fprintf(stderr, "write error.\n\n");
                    return -1;
                }
                continue;
            }
            else
            {
                fprintf(stderr, "write to fd error.\n\n");
                return -1;
            }
        }
        size += ret;
    }

    return size;
}

/**
 * @brief 
 *      send data to all fd by socket.
 * @param data  data ptr
 * @param len   data len
 * @return int 
 */
int TcpComm::_send_to_all(const Uint8* data, int len)
{
    if (data == NULL || len <= 0)
    {
        fprintf(stderr, "data is NULL or len <= 0 !\n");
        return 0;
    }
    std::lock_guard<std::mutex> lock(fd_set_mutex);
    std::set<int>::iterator it = fd_set.begin();
    for ( ; it != fd_set.end(); ++it)
    {
        _send_to_one(*it, data, len);
    }
    return 0;
}

/**
 * @brief 
 *      stsrt TCP server: 
 *          1.start send_to_handle_thread.
 *          2.start main_tcp_thread. init local socket and handle events.
 *          3.start send_to_client_thread.
 * @param path 
 */
void TcpComm::tcp_server_start(const std::string &path)
{
    if (state_flag)
    {
        fprintf(stdout, "[%s]:TCP server already has been running!\n", __func__);
        return;
    }
    
    state_flag = true;
    if(_tcp_server_init(path) < 0)
    {
        return;
    }

    /* ==================== send_to_handle_thread ==================== */
    std::thread send_to_handle_thread = std::thread([&]{
        printf("send_to_handle_thread start!\n");
        int fd;
        Uint8 *data = nullptr;
        int len;
        while (true)
        {
            queue_manager_ptr->pop_from_recv_queue(fd, &data, len);
            if (data != nullptr)
            {
                /* Virtual method Must be overridden */
                on_tcp_data_handle(fd, data, len);
            }
        }        
    });
    thread_array[SEND_TO_HANDLE_THREAD] = send_to_handle_thread.native_handle();
    send_to_handle_thread.detach();

    /* ==================== main_tcp_thread ==================== */
    std::thread main_tcp_thread = std::thread([&]{
        int event_num; //epoll event nummber
        Uint8 tmpBuff[MAX_BUFFER_SIZE] = {0}; //save buffer
        struct epoll_event events[MAX_EVENT_SIZE];
        int listen_fd = tcp_socket_ptr.get()->get_server_sfd();
        while (true)
        {
            /* epoll wait */
            event_num = epoll_util_ptr.get()->epoll_handle(events, MAX_EVENT_SIZE, -1);
            if (event_num < 0)
            {
                fprintf(stderr, "epollHandle error:%s\n",strerror(errno));
                continue;
            }

            Uint8 buffer[MAX_BUFFER_SIZE] = {0};
            /* handle */
            for (int i = 0; i < event_num; i++)
            {
                int fd = events[i].data.fd;
                int int_events = events[i].events;
                if((int_events & EPOLLERR) || (int_events & EPOLLRDHUP))
                {   //Disconnected
                    fprintf(stderr,"[%s]: EPOLLERR  or EPOLLRDHUP epollDel.\n", __func__);
                    std::lock_guard<std::mutex> lock(fd_set_mutex);
                    fd_set.erase(fd);
                    /* Virtual method Must be overridden */
                    client_remove_notify(fd);
                    epoll_util_ptr.get()->epoll_remove(fd);
                    continue;
                }
                if (fd == listen_fd) //accept request
                {
                    struct sockaddr_un client_addr;
                    socklen_t addr_len = sizeof(client_addr);
                    int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
                    if (client_fd < 0)
                    {
                        fprintf(stderr, "[%s]:Accept new client failure:%s\n", __func__, strerror(errno));
                        continue;
                    }
                    if (epoll_util_ptr.get()->epoll_add(client_fd) < 0)
                    {
                        fprintf(stderr, "[%s]:epollAdd clientFd failure:%s\n", __func__, strerror(errno));
                        continue;
                    }
                    std::lock_guard<std::mutex> lock(fd_set_mutex);
                    fd_set.insert(client_fd);
                    /* Virtual method Must be overridden */
                    client_add_notify(client_fd);
                    
                }
                else if ((int_events & EPOLLIN) != 0) //read request
                {
                    int len = recv(fd, buffer, MAX_BUFFER_SIZE, 0);
                    if (len > 0)
                    {
                        queue_manager_ptr->push_to_recv_queue(fd, buffer, len);
                    }
                    else
                    {
                        fprintf(stderr, "[%s]:epoll remove client fd:%d.\n", __func__, fd);
                        std::lock_guard<std::mutex> lock(fd_set_mutex);
                        fd_set.erase(fd);
                        /* Virtual method Must be overridden */
                        client_remove_notify(fd);
                        epoll_util_ptr.get()->epoll_remove(fd);
                    }
                    
                }
                else
                {
                    fprintf(stdout, "[%s]:unknown event request.\n", __func__);
                }

            }
            
        }
        
    });
    thread_array[MAIN_TCP_THREAD] = main_tcp_thread.native_handle();
    main_tcp_thread.detach();

    /* ==================== send_to_client_thread ==================== */
    std::thread send_to_client_thread = std::thread([&]{
        printf("send_to_client_thread start!\n");
        int fd;
        Uint8 *data = nullptr;
        int len;
        while (true)
        {
            queue_manager_ptr->pop_from_send_queue(fd, &data, len);

            if (data != nullptr)
            {
                if (fd >= 0 )
                {
                    _send_to_one(fd, data, len);
                }
                else
                {
                    _send_to_all(data, len);
                }
            }
        }
    });

    thread_array[SEND_TO_CLIENT_THREAD] = send_to_client_thread.native_handle(); 
    send_to_client_thread.detach();

}

/**
 * @brief 
 *      stop threads and release source.
 */
void TcpComm::tcp_server_stop()
{
    for (int i = 0; i < THREAD_NUM; i++)
    {
        pthread_cancel(thread_array[i]);
    }
    _tcp_resources_release();
    state_flag = false;
}

/**
 * @brief 
 *      push data to send_queue.
 * @param fd    des fd
 * @param data  data ptr
 * @param len   data len
 * @return int 0-always success.
 */
int TcpComm::write_to_send_queue(int fd, Uint8* data, int len)
{
    return queue_manager_ptr->push_to_send_queue(fd, data, len);
}

/**
 * @brief 
 *      virtual empty method.notify socket client add.
 *      if need know fd add,you should overwrite this method.
 * @param fd 
 */
void TcpComm::client_add_notify(int fd)
{
    printf("TcpComm::client_add_notify\n");
}

/**
 * @brief 
 *      virtual empty method.notify socket client remove.
 *      if need know fd remove,you should overwrite this method.
 * @param fd 
 */
void TcpComm::client_remove_notify(int fd)
{
    printf("TcpComm::client_remove_notify\n");
}

/**
 * @brief 
 *      handle data receive from socket. 
 *      this mothed will work in send_to_handle_thread.
 *      must be OVERWRITE!!!
 * @param fd 
 * @param data 
 * @param len 
 */
void TcpComm::on_tcp_data_handle(int fd, Uint8* data, int len)
{
    printf("TcpComm::on_tcp_data_handle\n");
}
