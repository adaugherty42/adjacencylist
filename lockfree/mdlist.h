#pragma once

#include <stdint.h>
#include "adjacencylist.h"
#include <vector>

// Dimension of 8 with a 32 bit keyspace gives each coordinate in base 16!
#define DIM 8
#define N std::pow(2, 32)

struct MDListNode
{
    uint32_t key;
    uint32_t k[DIM];
    Node *child[DIM];
    void *val;
};

class MDList
{

public:
    std::vector<uint32_t> keyToCoord(uint32_t key);
    ReturnValue FinishDelete(MDListNode *n, uint32_t, NodeDesc *nDesc)
    {
    }

    MDListNode *head;
    uint32_t basis;
    MDListNode list[];
};