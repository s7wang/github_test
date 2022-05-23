/**
 * @file QueueManager.h
 * @author wangs7
 * @brief Manager TCP receive queue and send queue
 * @version 0.1
 * @date 2021-12-28
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _QUEUEMANAGER_H__
#define _QUEUEMANAGER_H__

#include "includes.h"
#include "QueueForTcp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include <thread>
#include <mutex>


class QueueManager
{
private:
    /**
     * @brief 
     *      Internal encapsulation class
     * @param fd    date source fd
     * @param data  data ptr 
     * @param len   data len
     */
    class Packet
    {
    public:
        int fd;
        Uint8 *data; 
        int len;
    public:
        Packet():fd(-1), data(nullptr),len(0) { }
        Packet(int fd, const Uint8* data, int len) 
        {
            this->fd = fd;
            this->data = (Uint8*)malloc(len);
            memcpy(this->data, data, len);
            this->len = len;
        }
        Packet(Packet &&other)
        {
            this->fd = other.fd;
            this->data = other.data;
            other.data = nullptr;
            this->len = other.len;

        }
        Packet(const Packet &other)
        {
            this->fd = other.fd;
            this->data = (Uint8*)malloc(other.len);
            this->len = other.len;
            memcpy(this->data, other.data, other.len);
        }
        Packet &operator=(Packet &&other)
        {
            this->fd = other.fd;
            this->data = other.data;
            other.data = nullptr;
            this->len = other.len;
            
            return *this;
        }
        Packet &operator=(const Packet &other) 
        {
            if (this != &other)
            {
                this->fd = other.fd;
                this->data = (Uint8*)malloc(other.len);
                this->len = other.len;
                memcpy(this->data, other.data, other.len);
            }
            return *this;
        }

        virtual ~Packet()
        {
            if (data != nullptr)
            {
                free(data);
                data = nullptr;
            }
        }
        
    };
    
    QueueForTcp<Packet> recv_from_client_queue; //recv queue
    QueueForTcp<Packet> send_to_client_queue;   //send queue

private:
    QueueManager();
    QueueManager(QueueManager &&) = default;
    QueueManager(const QueueManager &) = default;
    QueueManager &operator=(QueueManager &&) = default;
    QueueManager &operator=(const QueueManager &) = default;

public:
    ~QueueManager();
    /**
     * @name get_instance
     * @brief Get the instance object
     * 
     * @return QueueManager* 
     */
    static QueueManager* get_instance()
    {
        static QueueManager* instance;
        static std::once_flag flag;
        std::call_once(flag, [&]{instance = new QueueManager;});   
        return instance;
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
    int push_to_recv_queue(int fd, Uint8* data, int len);

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
    int pop_from_recv_queue(int &fd, Uint8** data, int &len);

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
    int push_to_send_queue(int fd, Uint8* data, int len);

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
    int pop_from_send_queue(int &fd, Uint8** data, int &len);

    /**
     * @name clear
     * @brief 
     *      clear queues
     */
    void clear();

};





#endif // !_QUEUEMANAGER_H__