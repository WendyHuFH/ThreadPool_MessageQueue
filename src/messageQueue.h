//
// Created by WendyHu on 2020/12/3.
// 获取输入客票文件，将票写入消息队列中，reader线程专门去消息中获取客票，分发给processor线程

#ifndef OPSA_NEWSCHEDULENGINE_MESSAGEQUEUE_H
#define OPSA_NEWSCHEDULENGINE_MESSAGEQUEUE_H
#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <string>



template<typename T>
class messageQueue {
public:
    messageQueue(){}
    messageQueue (long _msgID):msgID(_msgID)
    {

    }
    virtual ~messageQueue( )
    {
    };
    messageQueue(const messageQueue &) =delete;
    messageQueue& operator = (const messageQueue&) = delete;

private:
    mutable std::mutex mut;
    std::queue<std::shared_ptr<T>> data_queue;
    std::condition_variable data_cond;
    long msgID;
public:
    /**
     * 发送消息
     */
    bool MsgSend(T new_value){
        std::shared_ptr<T> data(std::make_shared<T>(std::move(new_value)));
        std::lock_guard<std::mutex> lk(mut);
        size_t size=Size();
        bool res = false;
        data_queue.push(data);
        res = (size+1 == Size());
        data_cond.notify_one();
        return res;
    };
    /**
     * 接收消息
     */
    std::shared_ptr<T> MsgGet(){
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this](){return !data_queue.empty();});
        auto res = data_queue.front();
        data_queue.pop();
        return res;
    };

    /**
     * 将任务传出
     */
     bool try_pop(T &value){
         std::unique_lock<std::mutex> lk(mut);
         if(data_queue.empty()) return false;
         value = std::move(*data_queue.front());
         data_queue.pop();
         return true;
     }
    /**
     * 判断消息队列是否为空
     */
     bool Empty(){
        //std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
     };
     /**
      * 计算消息队列大小
      */
     size_t Size(){
         //std::lock_guard<std::mutex> lk(mut);
        return data_queue.size();
     };
     /**
      * 获取消息队列ID
      * @return
      */
     int getMsgID(){
         //std::lock_guard<std::mutex> lk(mut);
         return msgID;
     };

};

#endif//OPSA_NEWSCHEDULENGINE_MESSAGEQUEUE_H
