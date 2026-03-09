#pragma once
#include <iostream>
#include <queue>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <future>
#include <chrono>
#include "Thread_Pool.hpp"

namespace Producer_Consumer
{

    const int BUFFER_MAX = 10; 

    std::queue<int> buffer;
    std::mutex mtx;
    std::condition_variable cv_not_full, cv_not_empty;

   
    void produce()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }

    void consume()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    void Seq(int raw_products)
    {
        for (int i = 0; i < raw_products; i++)
        {
            produce();
            consume();
        }
    }


    void producer_task(int items)
    {
        for (int i = 0; i < items; i++)
        {
            produce();

            std::unique_lock<std::mutex> lock(mtx);

            cv_not_full.wait(lock, []()
            {
                return buffer.size() < BUFFER_MAX;
            });

            buffer.push(1);                
            cv_not_empty.notify_one();     
        }
    }


    void consumer_task(int items)
    {
        for (int i = 0; i < items; i++)
        {
            std::unique_lock<std::mutex> lock(mtx);

            cv_not_empty.wait(lock, []()
            {
                return !buffer.empty();
            });

            buffer.pop();                   
            cv_not_full.notify_one();       

            lock.unlock();
            consume();
        }
    }


    void Para(Thread_Pool &pool, int threads, int raw_products)
    {
        int p_threads = threads / 2;
        int c_threads = threads - p_threads;

        int items_p = raw_products / p_threads;
        int items_c = raw_products / c_threads;

        int extra_p = raw_products % p_threads;
        int extra_c = raw_products % c_threads;

        std::vector<std::future<void>> futs;

        for (int i = 0; i < p_threads; i++)
        {
            int k = items_p + (i == 0 ? extra_p : 0);
            futs.emplace_back(pool.submit([k] { producer_task(k); }));
        }

        for (int i = 0; i < c_threads; i++)
        {
            int k = items_c + (i == 0 ? extra_c : 0);
            futs.emplace_back(pool.submit([k] { consumer_task(k); }));
        }

       
        for (auto &f : futs)
            f.get();
    }

    static void test(Thread_Pool &pool, int threads)
    {
        int raw_products = 200;

        auto start = std::chrono::high_resolution_clock::now();
        Seq(raw_products);
        auto end = std::chrono::high_resolution_clock::now();
        auto Seq_time = std::chrono::duration<double>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        Para(pool, threads, raw_products);
        end = std::chrono::high_resolution_clock::now();
        auto Para_time = std::chrono::duration<double>(end - start).count();

        std::cout << "\nProducer–Consumer Pattern Benchmark\n";
        std::cout << "Sequential Time : " << Seq_time << " sec\n";
        std::cout << "Parallel Time   : " << Para_time << " sec\n";
        std::cout << "Speedup         : " << Seq_time / Para_time << "x\n";
        std::cout << "---------------------------------------\n";
    }
}
