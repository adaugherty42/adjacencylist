#pragma once
#include <stdint.h>
#include <stddef.h>
#include <iostream>
#include <unordered_set>
#include "lfstructs.h"
#include "mdlist.h"
#define F_adp 0x01
#define F_del 0x02
#define F_all 0x03

inline uintptr_t SetMark(void *p, uint32_t mark)
{
    return ((uintptr_t)(p) | mark);
}
inline uintptr_t ClearMark(void *p, uint32_t mark)
{
    return ((uintptr_t)(p) & ~mark);
}
inline uint32_t IsMarked(void *p, uint32_t mark)
{
    return (uintptr_t)p & mark;
}

class HelpStack
{
public:
    int top;
    NodeDesc **arr;

public:
    void Init()
    {
        top = -1;
        arr = new NodeDesc *[1000];
    }
    int size()
    {
        return top;
    }
    NodeDesc *Peek()
    {
        return arr[top];
    }
    NodeDesc *Pop()
    {
        return top > -1 ? arr[top--] : NULL;
    }
    bool Push(NodeDesc *nDesc)
    {
        if (top == 999)
        {
            return false;
        }
        else
        {
            arr[++top] = nDesc;
            return true;
        }
    }
    bool Contains(NodeDesc *nDesc)
    {
        std::cout << top << nDesc<< " contain start\n";
        for (int i = 0; i < top; i++)
        {
            if (arr[i] == nDesc)
            {
                
                return true;
            }
           // std::cout << i << "\n";
        }
        return false;
    }
};

struct Node
{
    NodeDesc *info;
    uint32_t key;
    MDList *list;
    Node *next;

    Node();
    void set(NodeDesc *i, uint32_t k, MDList *l, Node *n);
};

class AdjacencyList
{
public:
    AdjacencyList(int t);
    void init(int numElem, uint32_t num_ops);
    void InitializeVertices();
    void InitializeLists();
    Desc *AssignDesc(uint32_t size);
    bool ExecuteTransaction(NodeDesc *nDesc);
    void ExecuteOperations(NodeDesc *nDesc, uint32_t opid);
    Node *head;
    Node *tail;
    int numThreads;

    bool IsNodePresent(Node *n, uint32_t key);
    bool IsKeyPresent(NodeDesc *info, Desc *desc);
    enum SuccessValue UpdateInfo(Node *n, NodeDesc *info, bool wantKey);
    bool DeleteVertex(uint32_t vertex, NodeDesc *nDesc);
    bool InsertVertex(uint32_t vertex, NodeDesc *nDesc);
    bool InsertEdge(uint32_t vertex, uint32_t edge, NodeDesc *nDesc, uint32_t &currDim, uint32_t predDim, uint32_t k[]);
    bool DeleteEdge(uint32_t vertex, uint32_t edge, NodeDesc *nDesc, uint32_t &currDim, uint32_t predDim, uint32_t k[]);
    Node *FindVertex(uint32_t vertex, NodeDesc *nDesc);
    void LocatePred(Node *&pred, Node *&curr, uint32_t vertex);
    void MarkForDelete(Desc *d, std::unordered_set<NodeDesc *> toDelete);
    SuccessValue FinishDelete(MDListNode *n, uint32_t dc, NodeDesc *nDesc);
};