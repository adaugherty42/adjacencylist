#pragma once

#include <stdint.h>
#include <stddef.h>
#include "mdlist.h"
#include <unordered_set>
#include "mdlist.fwd.h"
#include "adjacencylist.fwd.h"

#define F_adp 0x01
#define F_del 0x02
#define F_all 0x03 // the paper calls it F_adp | F_del...overcomplicated for no reason

uintptr_t SetMark(void *p, uint32_t mark)
{
    return ((uintptr_t)(p) | mark);
}
uintptr_t ClearMark(void *p, uint32_t mark)
{
    return ((uintptr_t)(p) & ~mark);
}
uint32_t IsMarked(void *p, uint32_t mark)
{
    return (uintptr_t)p & mark;
}

// STL stack has no "contains" method...seriously!?
class HelpStack
{
public:
    uint32_t top;
    NodeDesc **arr;

public:
    void Init()
    {
        top = -1;
        arr = new NodeDesc *[1000];
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
        for (int i = 0; i < top; i++)
        {
            if (arr[i] == nDesc)
            {
                return true;
            }
        }
        return false;
    }
};

// LFTT Definitions
enum TxStatus
{
    ACTIVE,
    COMMITTED,
    ABORTED
};

// had to put Op at the end because compiler didn't like DeleteVertex
// i think bc somewwhere it's defined as a function or something
// it's 145 in the morning i'm not checking yet
enum OpType
{
    InsertVertexOp,
    DeleteVertexOp,
    InsertEdgeOp,
    DeleteEdgeOp,
    FindOp
};

// Needed for UpdateInfo
// can't put in header file bc enum
// hey now
enum SuccessValue
{
    Fail,
    Success,
    Retry
};

struct Operation
{
    OpType type;
    uint32_t key;
};

struct Desc
{
    uint32_t size;
    TxStatus status;
    int currentOp;
    Operation ops[];
};

struct NodeDesc
{
    Desc *desc;
    uint32_t opid;
    NodeDesc(Desc *desc, uint32_t opid)
        : desc(desc), opid(opid) {}
};

struct Node
{
    NodeDesc *info;
    uint32_t key;
    MDList *list;
    Node *next;

    Node(NodeDesc *info, uint32_t key, MDList *list, Node *next)
        : info(info), key(key), list(list), next(next) {}
};

class AdjacencyList
{
public:
    void InitializeVertices();
    void InitializeLists();
    Desc *AssignDesc(uint32_t size);
    bool ExecuteTransaction(NodeDesc *nDesc);
    void ExecuteOperations(NodeDesc *nDesc, uint32_t opid);
    Node *head;
    Node *tail;
    int numThreads;

    bool IsNodePresent(Node *n, uint32_t key);
    // bool IsKeyPresent(NodeDesc *info, Desc *desc);
    bool IsKeyPresent(NodeDesc *info, Desc *desc);
    enum SuccessValue UpdateInfo(Node *n, NodeDesc *info, bool wantKey);
    bool DeleteVertex(uint32_t vertex, NodeDesc *nDesc);
    bool InsertVertex(uint32_t vertex, NodeDesc *nDesc);
    // bool InsertEdge(uint32_t vertex, uint32_t edge, NodeDesc *nDesc, uint32_t opid);
    bool InsertEdge(uint32_t vertex, uint32_t edge, NodeDesc *nDesc, uint32_t &currDim, uint32_t predDim, uint32_t k[]);
    bool DeleteEdge(uint32_t vertex, uint32_t edge, NodeDesc *nDesc, uint32_t &currDim, uint32_t predDim, uint32_t k[]);
    Node *FindVertex(uint32_t vertex, NodeDesc *nDesc);
    void LocatePred(Node *&pred, Node *&curr, uint32_t vertex);
    void MarkForDelete(Desc *d, std::unordered_set<NodeDesc *> toDelete);
};