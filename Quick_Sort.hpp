#pragma once
#include <iostream>
#include <vector>
#include <atomic>
#include <chrono>
#include "Thread_Pool.hpp"

namespace Quick_Sort
{
    int partition(int l, int r, std ::vector<int> &v)
    {
        int m = l + (r - l) / 2;

        if (v[m] < v[l])
            std::swap(v[l], v[m]);
        if (v[r] < v[l])
            std::swap(v[l], v[r]);
        if (v[r] < v[m])
            std::swap(v[m], v[r]);

        std::swap(v[m], v[l]);
        int pivot = v[l];
        int i = l;
        for (int j = l + 1; j <= r; j++)
        {
            if (v[j] < pivot)
            {
                i++;
                std ::swap(v[i], v[j]);
            }
        }

        std ::swap(v[i], v[l]);

        return i;
    }
    void seq_sort(int l, int r, std::vector<int> &v)
    {
        if (l >= r)
            return;

        int p = partition(l, r, v);
        seq_sort(l, p - 1, v);
        seq_sort(p + 1, r, v);
    }
    void para_sort(int l, int r, std::vector<int> &v, Thread_Pool &pool, std ::atomic<int> &_threads)
    {
        if (l >= r)
            return;

        int n = r - l + 1;
        if (n < 2e4 || _threads <= 1)
        {
            seq_sort(l, r, v);
            return;
        }
        int p = partition(l, r, v);
        int left_size = p - l;
        int right_size = r - p;
        if (right_size > left_size)
        {
            auto right = pool.submit([&v, &pool, p, r, &_threads]
                                     {
            _threads.fetch_sub(1);
            para_sort(p+1,r,v,pool,_threads);
            _threads.fetch_add(1); });
            para_sort(l, p - 1, v, pool, _threads);
            right.get();
        }
        else
        {
            auto left = pool.submit([&v, &pool, p, l, &_threads]
                                    {
            _threads.fetch_sub(1);
            para_sort(l,p-1,v,pool,_threads);
            _threads.fetch_add(1); });
            para_sort(p + 1, r, v, pool, _threads);
            left.get();
        }
        return;
    }
    static void test(Thread_Pool &pool, std ::atomic<int> &_threads)
    {

        int n = 2e5;
        std ::vector<int> data_seq(n), data_para(n);
        for (int i = 0; i < n; i++)
        {
            data_seq[i] = rand();
            data_para[i] = data_seq[i];
        }
        auto start = std ::chrono ::high_resolution_clock ::now();
        seq_sort(0, n - 1, data_seq);
        auto end = std ::chrono::high_resolution_clock::now();
        auto Seq_time = std ::chrono ::duration<double>(end - start).count();
        start = std ::chrono ::high_resolution_clock ::now();
        para_sort(0, n - 1, data_para, pool, _threads);
        end = std ::chrono::high_resolution_clock::now();
        auto Para_time = std ::chrono ::duration<double>(end - start).count();
        for (int i = 0; i < n - 1; i++)
        {
            if (data_para[i] > data_para[i + 1])
            {
                std ::cout << "No\n";
                break;
            }
        }

        std ::cout << "Quick_Sort :-\n";
        std ::cout << "Seq_time -       " << Seq_time << "\n";
        std ::cout << "Para_time -      " << Para_time << "\n";
        std ::cout << "Boost -          " << Seq_time / Para_time << "x\n";
        std ::cout << "---------------------------------------\n";
    }
};
