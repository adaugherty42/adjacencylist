#pragma once

#include <stdint.h>
#include <stddef.h>
#include "mdlist.h"

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

    bool IsNodePresent(Node *n, uint32_t key)
    {
        return n->key == key;
    }

    // in paper where this is from, it's just Insert / Delete
    // need to determine whether it's vertex or edge
    // what to do that is different
    bool IsKeyPresent(NodeDesc *info, Desc *desc)
    {
        Operation op = info->desc->ops[info->opid];
        OpType opType = op.type;
        TxStatus status = info->desc->status;
        switch (status)
        {
        case ACTIVE:
        {
            if (info->desc == desc)
                return (opType == FindOp || opType == InsertVertexOp);
            else
                return (opType == FindOp || opType == DeleteVertexOp);
        }
        case COMMITTED:
            return (opType == FindOp || opType == InsertVertexOp);
        case ABORTED:
            return (opType == FindOp || opType == DeleteVertexOp);
        }
    }
    enum ReturnValue UpdateInfo(Node *n, NodeDesc *info, bool wantKey)
    {
        NodeDesc *oldDesc = n->info;
        if (IsMarked(reinterpret_cast<uint32_t>(oldDesc)))
        {
            // have to define DoDelete
            // DoDelete(n);
            return Retry;
        }
        if (oldDesc->desc != info->desc)
        {
            if (oldDesc->desc->ops[oldDesc->opid].type == DeleteVertexOp)
            {
                ExecuteOperation(oldDesc->desc, oldDesc->opid);
            }
            else
            {
                ExecuteOperation(oldDesc->desc, (oldDesc->opid) + 1);
            }
        }
        else if (oldDesc->opid >= info->opid)
        {
            return Success;
        }

        // not sure if this has the right second argument
        bool hasKey = IsKeyPresent(oldDesc, oldDesc->desc);
        if ((!hasKey && wantKey) || (hasKey && !wantKey))
            return Fail;
        if (info->desc->status != ACTIVE)
            return Fail;
        // write our own CAS for this
        // if (std::atomic_compare_exchange_strong(n->info, oldDesc, info))
        //     return Success;
        // else
        //     return Retry;
    }

    // vertex = Node key i think....
    bool DeleteVertex(uint32_t vertex, NodeDesc *nDesc)
    {
        Node *curr = head;
        Node *pred = NULL;
        while (true)
        {
            // have to define LocatePred
            // LocatePred(pred, curr, vertex);
            if (IsNodePresent(curr, vertex))
            {
                ReturnValue ret = UpdateInfo(curr, nDesc, true);
                if (ret != NULL)
                {
                    MDList *listPtr = curr->list;
                    MDList list = *listPtr;
                    // finish delete Node vs MDListNode is confusing
                    ret = list.FinishDelete(list.head, 0, nDesc);
                }
            }
        }
    }

private:
    uint32_t Mark = 0x1;
    Node *SetMark(uint32_t p)
    {
        return reinterpret_cast<Node *>(p | Mark);
    }
    Node *ClearMark(uint32_t p)
    {
        return reinterpret_cast<Node *>(p & ~Mark);
    }
    uint32_t IsMarked(uint32_t p)
    {
        return p & Mark;
    }
};