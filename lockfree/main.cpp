#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include "structs.h"
#include <vector>
#include <thread>
#include <iostream>
#include <chrono>

uint32_t num_threads = 4;
uint32_t num_transactions = 1000;
uint32_t ops_per_transaction = 2;

AdjacencyList *adjacencyList;

void run()
{
    // Execute the transactions
    for (int i = 0; i < num_transactions; i++)
    {
        // Each transaction has a descriptor with all the operations
        NodeDesc *nDesc = nDescAlloc.top();
        nDescAlloc.pop();
        Desc *desc = descAlloc.top();
        descAlloc.pop();

        // Now populate the descriptor's ops with random operations
        for (int j = 0; i < ops_per_transaction; j++)
        {
            int random = rand() * 10 + 1;

            // 1-3: Insert Vertex
            if (random < 4)
            {
                desc->ops[j].set(InsertVertexOp, rand() * std::pow(2, 32) + 1);
            }
            // 4-6: Delete Vertex
            else if (random < 7)
            {
                desc->ops[j].set(DeleteVertexOp, rand() * std::pow(2, 32) + 1);
            }
            // 7-8: Insert Edge
            else if (random < 9)
            {
                desc->ops[j].set(InsertEdgeOp, rand() + std::pow(2, 32) + 1);
            }
            // 9-10: Delete Edge
            else
            {
                desc->ops[j].set(DeleteEdgeOp, rand() + std::pow(2, 32) + 1);
            }
        }
        nDesc->desc = desc;
        adjacencyList->ExecuteTransaction(nDesc);
    }
}

int main(void)
{
    // Init creates preallocated pools of every kind of descriptor
    adjacencyList = new AdjacencyList(num_threads);
    adjacencyList->init(50000, ops_per_transaction);

    // Preallocate some nodes
    std::cout << "preallocate b\n";
    for (int i = 0; i < 1000; i++)
    {
        int random = rand() * 10000 + 1;
        NodeDesc *nDesc = nDescAlloc.top();
        nDescAlloc.pop();
        Desc *desc = descAlloc.top();
        descAlloc.pop();
        desc->ops[0].set(InsertVertexOp, random);

        nDesc->desc = desc;
        adjacencyList->ExecuteTransaction(nDesc);
    }
    std::cout << "preallocate a\n";


    std::vector<std::thread *> threads;
    threads.resize(num_threads);

    auto start = std::chrono::high_resolution_clock::now();

    // send our threads off to perform the number of transactions specified at the top of the file
    for (int j = 0; j < num_threads; j++)
    {
        threads[j] = new std::thread(run);
    }

    // when the threads complete their transactions, rejoin the main thread
    for (std::thread *t : threads)
    {
        if (t->joinable())
        {
            t->join();
        }
        auto stop = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double> >(stop - start);

        std::cout << "Time: " << time_span.count() << " seconds.";

        return 0;
    }
}