#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>
#include <chrono>
#include "Thread_Pool.hpp"

std::atomic<int> final_products = 0;
int buffer_count = 0;
std ::condition_variable cv_c;
std ::mutex mtx;
namespace Producer_Consumer
{

    void produce()
    {
        std ::this_thread::sleep_for(std ::chrono ::milliseconds(3));
    }
    void consume()
    {
        std ::this_thread::sleep_for(std ::chrono ::milliseconds(2));
    }
    void Seq(int raw_products)
    {
        for (int i = 0; i < raw_products; i++)
        {
            produce();
            consume();
        }
    }
    void produce_task(int items)
    {
        for (int i = 0; i < items; i++)
        {
            produce();
            
            final_products++;
            cv_c.notify_one();
        }
    }
    void consumer_task(int items)
    {
        for (int i = 0; i < items; i++)
        {
        
            std ::unique_lock<std ::mutex> lock(mtx);
            cv_c.wait(lock, []
                      { return final_products>0; });
            
            final_products--;
            lock.unlock();
            consume();
        }
    }
    void Para(Thread_Pool &pool, std::atomic<int> &_threads, int raw_products)
    {
        int p_threads = _threads / 2;
        int c_threads = _threads - p_threads;
        int items_p = raw_products / p_threads;
        int ex = raw_products % p_threads;
        int items_c = raw_products / c_threads;
        std ::vector<std ::future<void>> futs;

        for (int i = 0; i < p_threads; i++)
        {
            int k = items_p;
            if (i == 0)
                k += ex;
            futs.emplace_back(pool.submit([k]
                                          { produce_task(k); }));
        }
        ex = raw_products % c_threads;
        for (int i = 0; i < c_threads; i++)
        {
            int k = items_c;
            if (i == 0)
                k += ex;
            futs.emplace_back(pool.submit([k]
                                          { consumer_task(k); }));
        }
        for (auto &x : futs)
        {
            x.get();
        }
    }
    static void test(Thread_Pool &pool, std ::atomic<int> &_threads)
    {
        int raw_products = 200;
        auto start = std ::chrono::high_resolution_clock::now();
        Seq(raw_products);
        auto end = std ::chrono::high_resolution_clock::now();
        auto Seq_time = std ::chrono ::duration<double>(end - start).count();

        start = std ::chrono::high_resolution_clock::now();
        Para(pool, _threads, raw_products);
        end = std ::chrono::high_resolution_clock::now();

        auto Para_time = std ::chrono ::duration<double>(end - start).count();

        std ::cout << "Producer - Consumer -\n";
        std ::cout << "Seq_time -       " << Seq_time << "\n";
        std ::cout << "Para_time -      " << Para_time << "\n";
        std ::cout << "Boost -          " << Seq_time / Para_time << "x\n";
        std ::cout << "---------------------------------------\n";
    }
};
