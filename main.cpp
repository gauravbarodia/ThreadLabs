#include <iostream>
#include <thread>
#include <atomic>
#include "Thread_Pool.hpp"
#include "Merge_Sort.hpp"
#include "Quick_Sort.hpp"
#include "Matrix_Multiplication.hpp"
#include "Producer_Consumer.hpp"
#include "Fibonacci.hpp"
#include "Dijkstra.hpp"

int main()
{
    std::cout << "Parallel Algorithm Benchmarking Suite\n";
    std::cout << "---------------------------------------\n";

    int pool_size = std::thread::hardware_concurrency();
    // pool_size=4;
    Thread_Pool pool(pool_size);
    std ::atomic<int> _threads = pool_size;
    Merge_Sort::test(pool,_threads);
    Quick_Sort::test(pool,_threads);
    Matrix_Multiplication::test(pool,_threads);
    Fibonacci::test(pool,_threads);
    Dijkstra::test(pool, _threads);
    Producer_Consumer::test(pool,_threads);
    std::cout << "\nAll tests completed.\n";
    return 0;
}
