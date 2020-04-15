#pragma once
#include <stdint.h>
#include <vector>
#include <math.h>
#include "lfstructs.h"

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

    MDListNode();
    void set(uint32_t k, AdoptDesc *a, NodeDesc *i);
};

struct AdoptDesc
{
    MDListNode *curr;
    uint32_t dp;
    uint32_t dc;

    AdoptDesc();
    void set(MDListNode *cur, uint32_t p, uint32_t c);
};

class MDList
{

public:
    // use unsigned ints for everything...we don't need negatives here
    uint32_t *KeyToCoord(uint32_t key);
    void LocatePred(MDListNode *&curr, MDListNode *&pred, uint32_t &currDim, uint32_t &predDim, uint32_t k[]);
    void FinishInserting(MDListNode *n, AdoptDesc *ad);
    bool Find(uint32_t key);
    bool Delete(MDListNode *&curr, MDListNode *&pred, uint32_t dc, uint32_t dp);
    bool Insert(MDListNode *&node, MDListNode *&curr, MDListNode *&pred, uint32_t &dc, uint32_t &dp);
    void FillNewNode(MDListNode *&node, MDListNode *&curr, MDListNode *&pred, uint32_t &dc, uint32_t &dp);

    MDListNode *head;
    uint32_t basis;
    MDListNode list[];

    MDList();
    void set(MDListNode *h, uint32_t b);
};