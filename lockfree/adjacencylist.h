#pragma once

#include <stdint.h>
#include <stddef.h>
#include "mdlist.h"

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
enum ReturnValue
{
    Success,
    Fail,
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
    bool ExecuteOperation(Desc *desc, uint32_t opid);
    Node *head;
    Node *tail;
    int numThreads;

    bool IsNodePresent(Node *n, uint32_t key);
    bool IsKeyPresent(NodeDesc *info, Desc *desc);
    enum ReturnValue UpdateInfo(Node *n, NodeDesc *info, bool wantKey);
    bool DeleteVertex(uint32_t vertex, NodeDesc *nDesc);
    Node *FindVertex();
    // InsertEdge
    // DeleteEdge
    // DeleteVertex
};