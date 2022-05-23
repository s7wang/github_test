/**
 * @file QueueForTcp.h
 * @author wangs7
 * @brief Thread safe queues that support blocking and non blocking calls.
 * @version 0.1
 * @date 2022-01-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _QUEUEFORTCP_H__
#define _QUEUEFORTCP_H__

#include "includes.h"
#include <algorithm>
#include <memory>
#include <mutex>
#include <condition_variable>
template <typename T>
class QueueForTcp
{
private:
struct node
    {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };

    std::mutex head_mutex;
    std::unique_ptr<node> head;
    std::mutex tail_mutex;
    node* tail;
    std::condition_variable data_cond;

public:
    QueueForTcp()
    {
        head.reset(new node());
        tail = head.get();
    }
    QueueForTcp(QueueForTcp &&) = default;
    QueueForTcp(const QueueForTcp &) = delete;
    QueueForTcp &operator=(QueueForTcp &&) = default;
    QueueForTcp &operator=(const QueueForTcp &) = delete;
    ~QueueForTcp()
    {
        clear();
    }
private:
    node* _get_tail()
    {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        return tail;
    }

    std::unique_ptr<node> _pop_head()
    {
        std::unique_ptr<node> old_head = std::move(head);
        head = std::move(old_head->next);
        return old_head;
    }

    std::unique_lock<std::mutex> _wait_for_data()
    {
        std::unique_lock<std::mutex> head_lock(head_mutex);
        data_cond.wait(head_lock, [&] { return head.get() != _get_tail(); });
        return std::move(head_lock);
    }

    std::unique_ptr<node> _try_pop_head()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if (head.get() == _get_tail())
        {
            return nullptr;
        }
        return _pop_head();
    }

    std::unique_ptr<node> _wait_pop_head()
    {
        std::unique_lock<std::mutex> head_lock(_wait_for_data());
        return _pop_head();
    }
public:
    void push(T new_value)
    {
        std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
        
        std::unique_ptr<node> p(new node());
        
        {
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            node* const new_tail = p.get();
            tail->data = new_data;
            tail->next = std::move(p);
            tail = new_tail;
        }
        data_cond.notify_one();
    }
    std::shared_ptr<T> try_pop()
    {
        std::unique_ptr<node> old_head = _try_pop_head();
        return old_head ? old_head->data : std::shared_ptr<T> ();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::shared_ptr<node> const old_head = _wait_pop_head();
        return old_head->data;
    }

    bool empty()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        return (head.get() == _get_tail());
    }

    void clear()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        while (head.get() != _get_tail())
        {
            _pop_head().reset(); 
        }
    } 
};



#endif // !_QUEUEFORTCP_H__