#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>
#include <chrono>
#include "Thread_Pool.hpp"
std ::vector<std ::vector<std ::pair<int, int>>> Adj;

namespace Dijkstra
{
    std ::vector<int> Seq_Di(int start, int nodes)
    {

        std ::vector<int> dist(nodes,INT_MAX);
        std ::priority_queue<std ::pair<int, int>,std :: vector<std :: pair<int,int>>, std :: greater<>> pq;
        pq.push({0, start});
        dist[start] = 0;
        while (pq.size())
        {
            auto [cw, u] = pq.top();
            pq.pop();
            if (dist[u] < cw)
                continue;
            for (auto &[v, w] : Adj[u])
            {
                if (dist[v] > cw + w)
                {
                    dist[v] = cw + w;
                    pq.push({dist[v], v});
                }
            }
        }
        for (auto &x : dist)
        {
            if (x == INT_MAX)
                x = -1;
        }
        return dist;
    }
    std ::vector<int> Para_Di(int start, int nodes, Thread_Pool &pool, std ::atomic<int> &_threads)
    {
        std ::vector<int> dist(nodes, INT_MAX);
        dist[start] = 0;
        std ::vector<int> relaxed_nodes = {start};
        int t = _threads;
        while (relaxed_nodes.size())
        {
            
            int sz = relaxed_nodes.size();
            int block_size = (sz + t - 1) / t;
            std ::vector<std ::future<void>> futures;
            std ::vector<int> new_relaxed_node;
            std::mutex mtx;
            for (int st = 0; st < sz; st += block_size)
            {
                int end = std::min(st + block_size, sz);
                futures.emplace_back(pool.submit([&, st, end]()
                {
                    std ::vector<int> local_relaxed_nodes;
                    local_relaxed_nodes.reserve(end-st);
                    for(int i=st; i<end; i++){
                        int u=relaxed_nodes[i];
                        int d=dist[u];
                        for(auto &[v,w]:Adj[u]){
                            int new_dist=d+w;
                            int old_dist=dist[v];
                            if(old_dist>new_dist){
                                if (__atomic_compare_exchange_n(&dist[v], &old_dist, new_dist, false,__ATOMIC_RELAXED, __ATOMIC_RELAXED))
                                {
                                    local_relaxed_nodes.push_back(v);
                                }
                            }

                        }

                    } 
                    if(local_relaxed_nodes.size()){
                        std :: lock_guard<std :: mutex> lock(mtx);
                        new_relaxed_node.insert(new_relaxed_node.end(),local_relaxed_nodes.begin(),local_relaxed_nodes.end());
                    }
                }));
            }
            for(auto &fut:futures)fut.get();

            relaxed_nodes =std:: move( new_relaxed_node);
        }
        for (auto &x : dist)
        {
            if (x == INT_MAX)
                x = -1;
        }
        return dist;
    }
    static void test(Thread_Pool &pool, std ::atomic<int> &_threads)
    {
        int nodes = 2e5;
        int edges = 2e5;
        Adj.resize(nodes);
        int mod = 1e5;
        for (int i = 0; i < edges; i++)
        {
            int u = rand() % nodes;
            int v = rand() % nodes;
            int w = rand() % mod + 17;
            if (u != v)
            {
                Adj[u].push_back({v, w});
            }
        }
        auto start = std ::chrono::high_resolution_clock::now();
        std ::vector<int> dist_Seq = Seq_Di(0, nodes);
        auto end = std ::chrono::high_resolution_clock::now();

        auto Seq_time = std ::chrono ::duration<double>(end - start).count();
        start = std ::chrono::high_resolution_clock::now();
        std ::vector<int> dist_Para = Para_Di(0, nodes, pool, _threads);
        end = std ::chrono::high_resolution_clock::now();
        
        auto Para_time = std ::chrono ::duration<double>(end - start).count();

        std ::cout << "Dijkstra :-\n";
        std ::cout << "Seq_time -       " << Seq_time << "\n";
        std ::cout << "Para_time -      " << Para_time << "\n";
        std ::cout << "Boost -          " << Seq_time / Para_time << "x\n";
        std ::cout << "---------------------------------------\n";
    }
};
