/**
 * @file QueueManager.cpp
 * @author wangs7
 * @brief 
 * @version 0.1
 * @date 2021-12-28
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "QueueManager.h"

QueueManager::QueueManager()
{
}

QueueManager::~QueueManager()
{
}

/**
 * @name push_to_recv_queue
 * @brief 
 *      push data to recv_queue
 * @param fd socket fd
 * @param data data ptr
 * @param len data length
 * @return int 
 *      0 - always success.
 */
int QueueManager::push_to_recv_queue(int fd, Uint8* data, int len)
{
    recv_from_client_queue.push(Packet(fd, data, len));
    return 0;
}

/**
 * @name pop_from_recv_queue
 * @brief 
 *      pop data from recv_queue 
 * @param fd socket fd
 * @param data data ptr
 * @param len data length
 * @return int 
 *      0   success and The result will be placed in params. | 
 *      -1  fail and params will be reset.
 */
int QueueManager::pop_from_recv_queue(int &fd, Uint8** data, int &len)
{
    if ((*data) != nullptr)
    {
        free(*data);
        *data = nullptr;
    }
    /* Here is a blocking call, you can use try_pop as a non blocking call */
    std::shared_ptr<Packet> packet = recv_from_client_queue.wait_and_pop();
    if (packet == nullptr)
    {
        fd = -1;
        len = 0;
        return -1;
    }

    fd = packet.get()->fd;
    len = packet.get()->len;
    (*data) = (Uint8*)malloc(len);
    
    memcpy(*data, packet.get()->data, len);

    return 0;
}

/**
 * @name push_to_send_queue
 * @brief 
 *      push data to send_queue
 * @param fd socket fd
 * @param data data ptr
 * @param len data length
 * @return int 
 *      0 - always success.
 */
int QueueManager::push_to_send_queue(int fd, Uint8* data, int len)
{
    send_to_client_queue.push(Packet(fd, data, len));
    return 0;
}

/**
 * @name pop_from_send_queue
 * @brief 
 *      pop data from send_queue 
 * @param fd socket fd
 * @param data data ptr
 * @param len data length
 * @return int 
 *      0   success and The result will be placed in params. | 
 *      -1  fail and params will be reset.
 */
int QueueManager::pop_from_send_queue(int &fd, Uint8** data, int &len)
{
    if (*data != nullptr)
    {
        free(*data);
        *data = nullptr;
    }
    /* Here is a blocking call, you can use try_pop as a non blocking call */
    std::shared_ptr<Packet> packet = send_to_client_queue.wait_and_pop();
    if (packet == nullptr)
    {
        fd = -1;
        len = 0;
        return -1;
    }
    
    fd = packet.get()->fd;
    len = packet.get()->len;
    *data = (Uint8*)malloc(len);
    memcpy(*data, packet.get()->data, len);

    return 0;
}


/**
 * @name clear
 * @brief 
 *      clear queues
 */
void QueueManager::clear()
{
    recv_from_client_queue.clear();
    send_to_client_queue.clear();
}