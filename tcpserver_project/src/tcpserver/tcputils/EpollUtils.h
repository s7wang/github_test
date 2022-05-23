/**
 * @file EpollUtils.h
 * @author wangs7
 * @brief epoll function encapsulation
 * @version 0.1
 * @date 2021-12-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _EPOLLUTILS_H__
#define _EPOLLUTILS_H__

#include <sys/epoll.h>
#include "includes.h"

class EpollUtils
{
private:
    int _epoll_fd;
    int _fd_num;
public:
    EpollUtils();
    EpollUtils(EpollUtils &&) = default;
    EpollUtils(const EpollUtils &) = delete;
    EpollUtils &operator=(EpollUtils &&) = default;
    EpollUtils &operator=(const EpollUtils &) = delete;
    virtual ~EpollUtils();
    /**
     * @name epoll_init
     * @brief 
     *      epoll init
     * @param max_fd_num 
     * @return int [success] epoll_fd | 
     *             [fail] -1
     */    
    int epoll_init(int max_fd_num = MAX_LISTEN_FD);

    /**
     * @name epoll_add
     * @brief 
     *      add fd to epoll. 
     * @param fd 
     * @return int [success] fd(add)| 
     *             [fail] -1
     */
    int epoll_add(int fd);

    /**
     * @name epoll_remove
     * @brief 
     *      remove fd from epoll.
     * @param fd 
     * @return int [success] fd(remove)| 
     *             [fail] -1
     */
    int epoll_remove(int fd);

    /** 
     * @name epoll_handle
     * @brief 
     *      handle epoll events.
     * @param events 
     * @param maxevents 
     * @param timeout 
     * @return int [success] epoll_event_num |  
     *             [fail] -1
     */
    int epoll_handle(struct epoll_event *events, int maxevents, int timeout);
    
};



#endif // !_EPOLLUTILS_H__