#pragma once
#include <iostream>
#include <vector>
#include <atomic>
#include <chrono>
#include "Thread_Pool.hpp"

namespace Merge_Sort
{
    void merge(std ::vector<int> &v, int l, int m, int r)
    {
        std ::vector<int> left, right;
        for (int i = l; i <= m; i++)
        {
            left.push_back(v[i]);
        }
        for (int i = m + 1; i <= r; i++)
        {
            right.push_back(v[i]);
        }
        int i = 0, j = 0, k = 0;
        while (i < left.size() && j < right.size())
        {
            if (left[i] <= right[j])
            {
                v[l + k] = left[i++];
            }
            else
                v[l + k] = right[j++];
            k++;
        }
        while (i < left.size())
        {

            v[l + k] = left[i++];

            k++;
        }
        while (j < right.size())
        {
            v[l + k] = right[j++];
            k++;
        }
    }
    void seq_sort(std::vector<int> &v, int l, int r)
    {
        if (l >= r)
            return;

        int m = (r + l) / 2;
        seq_sort(v, l, m);
        seq_sort(v, m + 1, r);
        merge(v, l, m, r);
    }
    void para_sort(std::vector<int> &v, int l, int r, Thread_Pool &pool, std ::atomic<int> &_threads)
    {
        if (r <= l)
            return;

        int n = r - l + 1;
        if (_threads==0 || n<1e3 )
        {
            seq_sort(v, l, r);
            return;
        }

        int m = (l + r) / 2;
        auto left = pool.submit([&v, &pool, l, m,&_threads]()
                                { 
                                    _threads.fetch_sub(1);

                                    para_sort(v, l, m, pool,_threads ); 
                                    _threads.fetch_add(1); });

        para_sort(v, m + 1, r, pool, _threads);

        left.get();

        merge(v, l, m, r);
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
        seq_sort(data_seq, 0, n - 1);
        auto end = std ::chrono::high_resolution_clock::now();
        auto Seq_time = std ::chrono ::duration<double>(end - start).count();
        start = std ::chrono ::high_resolution_clock ::now();
        para_sort(data_para, 0, n - 1, pool, _threads);
        end = std ::chrono::high_resolution_clock::now();
        auto Para_time = std ::chrono ::duration<double>(end - start).count();
        std ::cout << "Merge_Sort :-\n";
        std ::cout << "Seq_time -       " << Seq_time << "\n";
        std ::cout << "Para_time -      " << Para_time << "\n";
        std ::cout << "Boost -          " << Seq_time / Para_time << "x\n";
        std ::cout << "---------------------------------------\n";
    }
};
