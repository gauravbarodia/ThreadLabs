#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <future>
#include <atomic>
#include <stdexcept>

class Thread_Pool
{
private:
    std ::vector<std ::thread> workers;
    std ::queue<std ::function<void()>> tasks;
    std ::mutex mtx;
    std ::condition_variable cv;
    std ::atomic<bool> stop;

public:
    Thread_Pool(int threads) : stop(false)
    {
        for (int i = 0; i < threads; i++)
        {
            workers.emplace_back([this]
            {
                while(true){
                    std :: function<void()>task;
                        {
                            std :: unique_lock<std :: mutex>lock(mtx);
                            cv.wait(lock,[this]{ return stop || !tasks.empty(); } );
                            if(stop && tasks.empty())return ;
                            task=std :: move(tasks.front());
                            tasks.pop();
                            
                        }
                        task();
                } 
            });
        }
    }
    ~Thread_Pool()
    {
        {
            std ::unique_lock<std ::mutex> lock(mtx);
            stop = true;
        }
        cv.notify_all();
        for (auto &worker : workers)
        {
            if (worker.joinable())
                worker.join();
        }
        workers.clear();
    }
    template <typename F, typename... Args>
    auto submit(F &&f, Args &&...args)
    {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task_ptr = std ::make_shared<std ::packaged_task<return_type()>>(
            std ::bind(std::forward<F>(f), std ::forward<Args>(args)...));
        std ::future<return_type> fut = task_ptr->get_future();
        {
            std ::unique_lock<std ::mutex> lock(mtx);
            tasks.emplace([task_ptr]()
                { 
                    (*task_ptr)(); 
                });
        }
        cv.notify_one();
        return fut;
    }
};