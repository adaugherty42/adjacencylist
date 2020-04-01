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
    MDListNode *child[DIM];
    AdoptDesc *aDesc;
    NodeDesc *info; // paper just uses this without declaring or explaining it.....
};

struct AdoptDesc
{
    MDListNode *curr;
    uint32_t dp;
    uint32_t dc;
};

class MDList
{

public:
    // use unsigned ints for everything...we don't need negatives here
    int *KeyToCoord(uint32_t key);
    void LocatePred(MDListNode *&curr, MDListNode *&pred, uint32_t &currDim, uint32_t &predDim, int k[]);
    void FinishInserting(MDListNode *n, AdoptDesc *ad);
    SuccessValue FinishDelete(MDListNode *n, uint32_t, NodeDesc *nDesc);
    bool Find(uint32_t key);
    bool Delete(MDListNode *&curr, MDListNode *&pred, uint32_t dc, uint32_t dp);
    bool Insert(MDListNode *&node, MDListNode *&curr, MDListNode *&pred, uint32_t &dc, uint32_t &dp);
    void FillNewNode(MDListNode *&node, MDListNode *&curr, MDListNode *&pred, uint32_t &dc, uint32_t &dp);

    MDListNode *head;
    uint32_t basis;
    MDListNode list[];
};