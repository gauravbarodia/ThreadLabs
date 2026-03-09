ThreadLabs – Multithreaded Algorithm Benchmarking

ThreadLabs is a C++ project built to explore how multithreading affects the performance of common algorithms.
It compares sequential, threaded, and ThreadPool-based executions to show where parallelism helps — and where it doesn't.

🚀 Features

Custom ThreadPool executor (task scheduling + futures)

Sequential vs parallel versions of:

Merge Sort

Quick Sort

Matrix Multiplication

Fast-Doubling Fibonacci

Producer–Consumer concurrency pattern

Dijkstra’s Shortest Path

Parallel Prefix Sum

Automatic timing + speedup calculation

📂 Structure
ThreadLabs/
│-- Thread_Pool.hpp
│-- Merge_Sort.hpp
│-- Quick_Sort.hpp
│-- Matrix_Multiplication.hpp
│-- Producer_Consumer.hpp
│-- Fibonacci.hpp
│-- Dijkstra.hpp
│-- Prefix_Sum.hpp
│-- main.cpp

🛠 Build & Run
g++ -std=c++17 -pthread main.cpp -o ThreadLabs
./ThreadLabs

📊 What This Project Demonstrates

Large workloads → parallelism improves performance

Small workloads → overhead makes multithreading slower

ThreadPool reduces cost by reusing threads

Shows practical behavior of:

mutexes, condition variables

atomics

task scheduling

CPU utilization

📘 Purpose

This project was created to experiment with and understand how multithreading impacts algorithm performance in real systems.
