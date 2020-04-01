#include "adjacencylist.h"
#include "mdlist.h"
#include <unordered_set>

thread_local HelpStack helpStack;

void AdjacencyList::ExecuteOperations(Desc *desc, uint32_t opid)
{
    SuccessValue ret = Success;
    std::unordered_set<void *> toDelete;

    // if helpstack contains desc, CAS(&desc.flag, active, aborted); return
    if (helpStack.Contains(desc))
    {
        // Can't use __atomic_compare_exchange_n here because it expects a pointer for the second argument
        // and you can't point to an enum constant value, so we have to use this GCC-only monstrosity. Hilarious.
        __sync_bool_compare_and_swap(&desc->status, ACTIVE, ABORTED);
        return;
    }

    // Goodbye livelock!
    helpStack.Push(desc);

    while (desc->status == ACTIVE && opid < desc->size)
    {
        Operation op = desc->ops[opid];
        switch (op.type)
        {
        case InsertVertexOp:
            // ret = InsertVertex()...
            break;
        case InsertEdgeOp:
            // ret = InsertEdge()...
            break;
        case DeleteVertexOp:
            // ret = DeleteVertex()...
            break;
        case DeleteEdgeOp:
            // ret = DeleteEdge()...
            break;
        case FindOp:
            // ret = Find()...
            break;
        }
        opid++;
    }

    // Transaction has either succeeded or failed at this point, so pop descriptor off stack.
    helpStack.Pop();

    // Do housekeeping
    if (ret == Success)
    {
        // All operations succeeded.
        if (__sync_bool_compare_and_swap(&desc->status, ACTIVE, COMMITTED))
        {
            //MarkForDeletion(toDelete)
        }
    }
    else
    { // Some operation failed...transaction must abort
        __sync_bool_compare_and_swap(&desc->status, ACTIVE, ABORTED);
    }
}

bool AdjacencyList::ExecuteTransaction(Desc *desc)
{
    helpStack.Init();
    ExecuteOperations(desc, 0);
}

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
            ExecuteOperations(oldInfo->desc, oldInfo->opid);
        }
        else
        {
            ExecuteOperations(oldInfo->desc, (oldInfo->opid) + 1);
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

bool AdjacencyList::DeleteVertex(uint32_t vertex, NodeDesc *nDesc)
{
    Node *curr = head;
    Node *pred = NULL;
    SuccessValue ret;
    while (true)
    {
        LocatePred(pred, curr, vertex);
        if (IsNodePresent(curr, vertex))
        {
            ret = UpdateInfo(curr, nDesc, true);
            if (ret != NULL)
            {
                MDList *listPtr = curr->list;
                MDList list = *listPtr;
                ret = list.FinishDelete(list.head, 0, nDesc);
            }
            if (ret)
                return true;
            else
                return false;
        }
    }
}

bool AdjacencyList::InsertVertex(uint32_t vertex, NodeDesc *nDesc)
{
    Node *curr = head;
    Node *pred = NULL;
    SuccessValue ret;
    while (true)
    {
        LocatePred(pred, curr, vertex);
        if (!IsNodePresent(curr, vertex))
        {
            ret = UpdateInfo(curr, nDesc, true);
            if (ret != NULL)
            {
                MDList *mdlist = new MDList();
                //In this case, it al- locates a new vertex and inserts it into the list using Compare-And-Swap to change pred.next to curr.
                Node *toInsert = new Node(nDesc, vertex, mdlist, pred->next);
                if (__atomic_compare_exchange_n(&pred->next, curr, toInsert, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
                    return true;
                else
                    return false;
            }
        }
    }
}

void AdjacencyList::LocatePred(Node *&pred, Node *&curr, uint32_t vertex)
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
                ExecuteOperations(cDesc->desc, cDesc->desc->currentOp + 1);
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