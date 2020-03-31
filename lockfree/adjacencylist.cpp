#include "adjacencylist.h"
#include "mdlist.h"

bool AdjacencyList::IsNodePresent(Node *n, uint32_t key)
{
    return n->key == key;
}

// in paper where this is from, it's just Insert / Delete
// need to determine whether it's vertex or edge
// what to do that is different
bool AdjacencyList::IsKeyPresent(NodeDesc *info, Desc *desc)
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

enum ReturnValue AdjacencyList::UpdateInfo(Node *n, NodeDesc *info, bool wantKey)
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
    bool hasKey = IsKeyPresent(oldInfo, oldInfo->desc);
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