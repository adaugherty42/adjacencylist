#include "adjacencylist.h"
#include "mdlist.h"

bool AdjacencyList::IsNodePresent(Node *n, uint32_t key)
{
    return n->key == key;
}

// bool AdjacencyList::IsKeyPresent(NodeDesc *info, Desc *desc)
bool AdjacencyList::IsKeyPresent(NodeDesc *info)
{
    Operation op = info->desc->ops[info->opid];
    OpType opType = op.type;
    TxStatus status = info->desc->status;
    switch (status)
    {
    // case ACTIVE:
    // {
    //     if (info->desc == desc)
    //         return (opType == FindOp || opType == InsertVertexOp || opType == InsertEdgeOp);
    //     else
    //         return (opType == FindOp || opType == DeleteVertexOp || opType == DeleteEdgeOp);
    // }
    case COMMITTED:
        return (opType == FindOp || opType == InsertVertexOp || opType == InsertEdgeOp);
    case ABORTED:
        return (opType == FindOp || opType == DeleteVertexOp || opType == DeleteEdgeOp);
    }
}

enum SuccessValue AdjacencyList::UpdateInfo(Node *n, NodeDesc *info, bool wantKey)
{
    NodeDesc *oldInfo = n->info;
    if (IsMarked(oldInfo, F_adp))
    {
        // have to define DoDelete
        // DoDelete(n);
        return Retry;
    }
    if (oldInfo->desc != info->desc)
    {
        if (oldInfo->desc->ops[oldInfo->opid].type == DeleteVertexOp)
        {
            ExecuteOperation(oldInfo->desc, oldInfo->opid);
        }
        else
        {
            ExecuteOperation(oldInfo->desc, (oldInfo->opid) + 1);
        }
    }
    else if (oldInfo->opid >= info->opid)
    {
        return Success;
    }

    // not sure if this has the right second argument
    bool hasKey = IsKeyPresent(oldInfo);
    if ((!hasKey && wantKey) || (hasKey && !wantKey))
        return Fail;
    if (info->desc->status != ACTIVE)
        return Fail;
    if (__atomic_compare_exchange_n(&n->info, oldInfo, info, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
        return Success;
    else
        return Retry;
}

// vertex = Node key i think....
bool AdjacencyList::DeleteVertex(uint32_t vertex, NodeDesc *nDesc)
{
    Node *curr = head;
    Node *pred = NULL;
    while (true)
    {
        // have to define LocatePred
        // LocatePred(pred, curr, vertex);
        if (IsNodePresent(curr, vertex))
        {
            SuccessValue ret = UpdateInfo(curr, nDesc, true);
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

void ExecuteOps(Desc *desc, uint32_t opid)
{
}

void LocatePred(Node *&pred, Node *&curr, uint32_t vertex)
{
    while (curr->key < vertex)
    {
        pred = curr;
        curr = curr->next;
    }
}

Node *AdjacencyList::FindVertex(uint32_t vertex, NodeDesc *nDesc, uint32_t opid)
{
    Node *curr = head;
    Node *pred = NULL;
    while (true)
    {
        LocatePred(pred, curr, vertex);
        if (IsNodePresent(curr, vertex))
        {
            NodeDesc *cDesc = curr->info;
            if (cDesc != nDesc)
            {
                ExecuteOps(cDesc->desc, cDesc->desc->currentOp + 1);
            }
            if (IsKeyPresent(cDesc))
            {
                if (nDesc->desc->status != ACTIVE)
                {
                    return NULL;
                }
                return curr;
            }
        }
        return NULL;
    }
}