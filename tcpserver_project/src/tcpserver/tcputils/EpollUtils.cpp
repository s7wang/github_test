/**
 * @file EpollUtils.cpp
 * @author wangs7
 * @brief 
 * @version 0.1
 * @date 2021-12-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "EpollUtils.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>

EpollUtils::EpollUtils():_epoll_fd(-1), _fd_num(0)
{

}

EpollUtils::~EpollUtils()
{
}

/**
 * @name epoll_init
 * @brief 
 *      epoll init
 * @param max_fd_num 
 * @return int [success] epoll_fd | 
 *             [fail] -1
 */
int EpollUtils::epoll_init(int max_fd_num)
{
    _epoll_fd = epoll_create(max_fd_num);
    if (_epoll_fd < 0)
    {
        fprintf(stderr, "[%s]:epoll_create failed:%s.\n", __func__, strerror(errno));
        return -1;
    }
    return _epoll_fd;
}

/**
 * @name epoll_add
 * @brief 
 *      add fd to epoll. 
 * @param fd 
 * @return int [success] fd(add)| 
 *             [fail] -1
 */
int EpollUtils::epoll_add(int fd)
{
    struct epoll_event ev;
    if (_epoll_fd < 0 || fd < 0)
    {
        fprintf(stderr,"[%s]:epoll_fd or fd < 0.\n", __func__);
        return -1;
    }


    if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL)|O_NONBLOCK ) < 0)
    {
        fprintf(stderr,"[%s]:fcntl error:%s.\n", __func__, strerror(errno));
        return -1;
    }
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0)
    {
        fprintf(stderr,"[%s]:epoll_ctl error:%s.\n", __func__, strerror(errno));
        return -1;
    }
    _fd_num++;
    fprintf(stdout, "[%s]:epoll add fd = %d, fd nums:%d in total.\n", __func__, fd, _fd_num);
    return fd;
}

/**
 * @name epoll_remove
 * @brief 
 *      remove fd from epoll.
 * @param fd 
 * @return int [success] fd(remove)| 
 *             [fail] -1
 */
int EpollUtils::epoll_remove(int fd)
{
    if (_epoll_fd < 0 || fd < 0)
    {
        fprintf(stderr,"[%s]:epoll_fd or fd < 0.\n", __func__);
        return -1;
    }
    if(epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
    {
        fprintf(stderr,"[%s]:epoll_ctl error:%s.\n", __func__, strerror(errno));
        return -1;
    }
    close(fd); _fd_num--;
    fprintf(stdout, "[%s]:epoll remove fd = %d, there are %d fds in total.\n", __func__, fd, _fd_num);
    return fd;
}

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
int EpollUtils::epoll_handle(struct epoll_event *events, int maxevents, int timeout)
{
    int epoll_event_num = epoll_wait(_epoll_fd, events, maxevents, timeout);
    if (epoll_event_num < 0)
    {
        fprintf(stderr,"[%s]:epoll_wait error:%s.\n", __func__, strerror(errno));
        return -1;
    }

    return epoll_event_num;
}