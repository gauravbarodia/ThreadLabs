#pragma once
#include <iostream>
#include <vector>
#include <atomic>
#include <chrono>
#include "Thread_Pool.hpp"
int mod = 1e9 + 7;
namespace Fibonacci
{

    long long Seq_Fib(int n)
    {
        if (n <= 1)
            return n;
        if (n == 2)
            return 3;

        int k = n / 2;
        long long a = Seq_Fib(k);
        long long b = Seq_Fib(k + 1);

        if (n % 2 == 0)
            return (a * a % mod + b * b % mod) % mod;
        return (2 * (a * b % mod) % mod + b * b % mod) % mod;
    }
    long long Para_Fib(int n, Thread_Pool &pool, std ::atomic<int> &_threads)
    {
        if (n <= 1)
            return n;

        if (n == 2)
            return 3;

        if (_threads <= 1 || n < 1e3)
        {
            return Seq_Fib(n);
        }
        int k = n / 2;

        auto fut_b = pool.submit([k, &pool, &_threads]()
            {
                _threads.fetch_sub(1); 
                return Para_Fib(k + 1, pool, _threads); 
               
            });
        long long a = Para_Fib(k, pool, _threads);
        long long b = fut_b.get();
         _threads.fetch_add(1); 
        if (n % 2 == 0)
            return (a * a % mod + b * b % mod) % mod;
        return (2 * (a * b % mod) % mod + b * b % mod) % mod;
    }
    static void test(Thread_Pool &pool, std ::atomic<int> &_threads)
    {
        int n = 1e9;
        auto start = std ::chrono::high_resolution_clock::now();
        long long s_nth = Seq_Fib(n);
        auto end = std ::chrono::high_resolution_clock::now();

        auto Seq_time = std ::chrono ::duration<double>(end - start).count();
        start = std ::chrono::high_resolution_clock::now();
        long long p_nth = Para_Fib(n, pool, _threads);
        end = std ::chrono::high_resolution_clock::now();

        auto Para_time = std ::chrono ::duration<double>(end - start).count();

        std ::cout << "Fast Doubling Fibonacci :-\n";
        std ::cout << "Seq_time -       " << Seq_time << "\n";
        std ::cout << "Para_time -      " << Para_time << "\n";
        std ::cout << "Boost -          " << Seq_time / Para_time << "x\n";
        std ::cout << "---------------------------------------\n";
    }
};
