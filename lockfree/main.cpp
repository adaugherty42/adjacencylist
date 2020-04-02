#include <stdint.h>
#include <stdlib.h>
#include <vector.h>
#include "adjacencylist.h"
#include "structs.h"
#include "mdlist.h"


uint32_t num_threads = 4;
uint32_t num_transactions = 1000;
uint32_t ops_per_transaction = 2;

void run(AdjacencyList *adjacencyList)
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
                desc->ops[j].set(InsertVertexOp, rand() * N + 1);
            }
            // 4-6: Delete Vertex
            else if (random < 7)
            {
                desc->ops[j].set(DeleteVertexOp, rand() * N + 1);
            }
            // 7-8: Insert Edge
            else if (random < 9)
            {
                desc->ops[j].set(InsertEdgeOp, rand() + N + 1);
            }
            // 9-10: Delete Edge
            else
            {
                desc->ops[j].set(DeleteEdgeOp, rand() + N + 1);
            }
        }
        nDesc->desc = desc;
        adjacencyList->ExecuteTransaction(nDesc);
    }
}

int main()
{
    AdjacencyList* adjacencyList = AdjacencyList();
    adjacencyList->init(50000);

    // Preallocate some nodes
    for (int i = 0; i < 1000; i++)
    {
        int random = rand() * 10000 + 1;
        NodeDesc *nDesc = nDescAlloc.top();
        nDescAlloc.pop();
        Desc *desc = descAlloc.top();
        descAlloc.pop();
        desc->ops[0].set(InsertVertexOp, random);
        nDesc->desc = desc;
        adjacencyList.ExecuteTransaction(nDesc);
    }

    vector<thread*> threads;
    threads.resize(num_threads);

    auto start = high_resolution_clock::now();

    for(int j = 0; j < num_threads; j++){
        threads.[j] = new thread(run, adjacencylist);
    }

    for (thread *t : threads) {
        if (t->joinable()) {
            t->join();
    }
    auto stop = high_resolution_clock::now();

    duration<double> time_span = duration_cast<duration<double>>(stop - start);

    std::cout << "Time: " << time_span.count() << " seconds.";

    return 0;
}