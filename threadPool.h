//
// Created by WendyHu on 2020/12/18.
// 基于工作任务队列，创建线程池

#ifndef OPSA_NEWSCHEDULENGINE_THREADPOOL_H
#define OPSA_NEWSCHEDULENGINE_THREADPOOL_H
#include "../safeQueue/messageQueue.h"
//#include "../auditManager.h"
#include <future>
#include <atomic>

class threadPool {
public:
    threadPool();
    ~threadPool();
    std::vector<std::future<bool>> threads;
    /**
    * 提交任务到工作队列
    * @tparam functionType 任务类型
    * @param f 任务
    * @return 任务结果类型
    */
    template<typename functionType,typename... Args>
    std::future<typename std::result_of<functionType(Args...)>::type> submit(functionType &&f, Args&& ...args) {
        using result_type = typename std::result_of<functionType(Args&& ...)>::type;
        std::packaged_task<result_type()> task(std::move(std::bind(std::forward<functionType>(f),std::forward<Args>(args)...)));
        std::future<result_type> res(task.get_future());
        work_queue.MsgSend(std::move(task));
        return res;
    }

    bool workThread();

private:
    std::atomic_bool done;
    messageQueue<std::packaged_task<bool()>> work_queue;
    

};



/**
 * 创建特定数目的线程池，添加任务到空闲线程运行
 */
inline threadPool::threadPool() :done(false) {
    unsigned const thread_count = std::thread::hardware_concurrency();
    try{
        for(unsigned i=0;i<thread_count;i++){
            threads.push_back(std::async(&threadPool::workThread,this));
        }
    }
    catch (...) {
        done = true;
        throw;
    }
}
/**
 * 任务结束，线程池销毁
 */
inline threadPool::~threadPool() {
    done = true;
    //for (auto& res : threads) {
    //    res.get();
    //}
}

/**
 * 从工作队列中取任务并运行
 */
inline bool threadPool::workThread() {
    while(!done){
        std::packaged_task<bool()> task;
        if(work_queue.try_pop(task))
        {
            task();
        }
        else
        {
            std::this_thread::yield();
            //done=true;
        }
    }
    return true;
}

#endif//OPSA_NEWSCHEDULENGINE_THREADPOOL_H
