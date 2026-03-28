#pragma once
#include <iostream>
#include <vector>
#include <atomic>
#include <chrono>
#include "Thread_Pool.hpp"

namespace Matrix_Multiplication
{

    std::vector<std ::vector<int>> Seq_Mul(std::vector<std ::vector<int>> &A, std::vector<std ::vector<int>> &B)
    {
        int A_Row = A.size(), A_Col = A[0].size();
        int B_Row = B.size(), B_Col = B[0].size();
        if (A_Col != B_Row)
        {
            return {};
        }
        std::vector<std ::vector<int>> C(A_Row, std ::vector<int>(B_Col,0));
        for (int i = 0; i < A_Row; i++)
        {
            for (int j = 0; j < B_Col; j++)
            {
                for (int k = 0; k < A_Col; k++)
                {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
        return C;
    }
    std::vector<std ::vector<int>> Para_Mul(std::vector<std ::vector<int>> &A, std::vector<std ::vector<int>> &B, Thread_Pool &pool, std ::atomic<int> &_threads)
    {
        int A_Row = A.size(), A_Col = A[0].size();
        int B_Row = B.size(), B_Col = B[0].size();
        if (A_Col != B_Row)
        {
            return {};
        }
        std::vector<std ::vector<int>> C(A_Row, std ::vector<int>(B_Col,0));
        int d = _threads;
        int block_size = (A_Row + d - 1) / d;
        
        std ::vector<std ::future<void>> blocks;
        for (int start = 0; start < A_Row; start += block_size)
        {
            int end = std::min(start + block_size, A_Row);
            blocks.emplace_back(pool.submit([start, end, B_Col, A_Col, &C, &A, &B]()
            {
                for (int i = start; i < end; i++)
                {
                    for (int j = 0; j < B_Col; j++)
                    {
                        for (int k = 0; k < A_Col; ++k)
                        {
                            C[i][j] += A[i][k] * B[k][j];
                        }
                    }
                }    
            }
            ));
        }
        for(auto &block:blocks)block.get();
        return C;
    }
    static void test(Thread_Pool &pool, std ::atomic<int> &_threads)
    {
        int Row = 400, Col = 400;

        std::vector<std ::vector<int>> A(Row, std ::vector<int>(Col)), B(Row, std ::vector<int>(Col));
        for (int i = 0; i < Row; i++)
        {
            for (int j = 0; j < Col; j++)
            {
                A[i][j] = rand() % 400;
                B[i][j] = rand() % 400;
            }
        }
        auto start = std ::chrono::high_resolution_clock::now();
        std::vector<std ::vector<int>> Seq_C = Seq_Mul(A, B);
        auto end = std ::chrono::high_resolution_clock::now();

        auto Seq_time = std ::chrono ::duration<double>(end - start).count();
        start = std ::chrono::high_resolution_clock::now();
        std::vector<std ::vector<int>> Para_C = Para_Mul(A, B, pool, _threads);
        end = std ::chrono::high_resolution_clock::now();

        auto Para_time = std ::chrono ::duration<double>(end - start).count();

        std ::cout << "Matrix_Multiplication :-\n";
        std ::cout << "Seq_time -       " << Seq_time << "\n";
        std ::cout << "Para_time -      " << Para_time << "\n";
        std ::cout << "Boost -          " << Seq_time / Para_time << "x\n";
        std ::cout << "---------------------------------------\n";
    }
};
