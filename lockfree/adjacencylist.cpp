#include "structs.h"
#include <unordered_set>
#include <pthread.h>
#include <iostream>

__thread HelpStack helpStack;

AdjacencyList::AdjacencyList(int t)
{
    head = new Node();
    head->set(NULL, 0, NULL, NULL);
    tail = new Node();
    tail->set(NULL, 0xFFFFFFFF, NULL, NULL);
    head->next = tail;
    numThreads = t;
}

void AdjacencyList::init(int numElem, uint32_t num_ops)
{
    std::cout << "init start \n";

    nodeAlloc = std::stack<Node *>();
    descAlloc = std::stack<Desc *>();
    nDescAlloc = std::stack<NodeDesc *>();
    mdlistAlloc = std::stack<MDList *>();
    mdlNodeAlloc = std::stack<MDListNode *>();
    aDescAlloc = std::stack<AdoptDesc *>();

    for (int i = 0; i < numElem; i++)
    {
        Node* node = new Node();
        nodeAlloc.push(node);
        Desc* desc = new Desc(num_ops);
        descAlloc.push(desc);
        NodeDesc* nDesc = new NodeDesc();
        nDescAlloc.push(nDesc);
        MDList* mdList = new MDList();
        mdlistAlloc.push(mdList);
        MDListNode* mdlNode = new MDListNode();
        mdlNodeAlloc.push(mdlNode);
        AdoptDesc* aDesc = new AdoptDesc();
        aDescAlloc.push(aDesc);
    }
    
    std::cout << "init end \n";
}

 

void AdjacencyList::ExecuteOperations(NodeDesc *nDesc, uint32_t opid)
{
    SuccessValue ret = Success;
    std::unordered_set<NodeDesc *> toDelete;

    // if helpstack contains desc, CAS(&desc.flag, active, aborted); return
    if (helpStack.Contains(nDesc))
    {
        __sync_bool_compare_and_swap(&nDesc->desc->status, ACTIVE, ABORTED);
        return;
    }

    // Goodbye livelock!
    helpStack.Push(nDesc);

    while (nDesc->desc->status == ACTIVE && ret != Fail && opid < nDesc->desc->size)
    {
        Operation op = nDesc->desc->ops[opid];
        NodeDesc *nd = nDescAlloc.top();
        nDescAlloc.pop();
        nd->set(nDesc->desc, opid);

        switch (op.type)
        {
        case InsertVertexOp:
            ret = InsertVertex(op.key, nd) ? Success : Fail;
            break;
        case InsertEdgeOp:
            // ret = InsertEdge()...
            break;
        case DeleteVertexOp:
            ret = DeleteVertex(op.key, nd) ? Success : Fail;
            break;
        case DeleteEdgeOp:
            // ret = DeleteEdge()...
            break;
        case FindOp:
            ret = FindVertex(op.key, nd) != NULL ? Success : Fail; // replace with find
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
        if (__sync_bool_compare_and_swap(&nDesc->desc->status, ACTIVE, COMMITTED))
        {
            MarkForDelete(nDesc->desc, toDelete);
        }
    }
    else
    { // Some operation failed...transaction must abort
        __sync_bool_compare_and_swap(&nDesc->desc->status, ACTIVE, ABORTED);
    }
}

bool AdjacencyList::ExecuteTransaction(NodeDesc *nDesc)
{
    helpStack.Init();
    ExecuteOperations(nDesc, 0);
}

bool AdjacencyList::IsNodePresent(Node *n, uint32_t key)
{
    return n->key == key;
}

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
            return (opType == FindOp || opType == InsertVertexOp || opType == InsertEdgeOp);
        else
            return (opType == FindOp || opType == DeleteVertexOp || opType == DeleteEdgeOp);
    }
    case COMMITTED:
        return (opType == FindOp || opType == InsertVertexOp || opType == InsertEdgeOp);
    case ABORTED:
        return (opType == FindOp || opType == DeleteVertexOp || opType == DeleteEdgeOp);
    }
}

enum SuccessValue AdjacencyList::UpdateInfo(Node *n, NodeDesc *info, bool wantKey)
{
        std::cout << "UPDATEINFORMation\n";
    NodeDesc *oldInfo = n->info;
    if (IsMarked(oldInfo, F_adp))
    {
        // have to define DoDelete
        // DoDelete(n);
        return Retry;
    }
        std::cout << "waoh\n";
    if (oldInfo->desc != info->desc)
    {
        if (oldInfo->desc->ops[oldInfo->opid].type == DeleteVertexOp)
        {
            ExecuteOperations(oldInfo, oldInfo->opid);
        }
        else
        {
            ExecuteOperations(oldInfo, (oldInfo->opid) + 1);
        }
    }
    else if (oldInfo->opid >= info->opid)
    {
        return Success;
    }


    // not sure if this has the right second argument
    bool hasKey = IsKeyPresent(oldInfo, info->desc);
    if ((!hasKey && wantKey) || (hasKey && !wantKey))
        return Fail;
    if (info->desc->status != ACTIVE)
        return Fail;
    if (__atomic_compare_exchange_n(&n->info, &oldInfo, info, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
        return Success;
    else
        return Retry;
}

bool AdjacencyList::DeleteVertex(uint32_t vertex, NodeDesc *nDesc)
{
    Node *curr = head;
    Node *pred = nodeAlloc.top();
    nodeAlloc.pop();
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
                ret = FinishDelete(list.head, 0, nDesc);
            }
            if (ret)
                return true;
            else
                return false;
        }
    }
}

SuccessValue AdjacencyList::FinishDelete(MDListNode *n, uint32_t dc, NodeDesc *nDesc)
{
    return Fail;
}

bool AdjacencyList::InsertVertex(uint32_t vertex, NodeDesc *nDesc)
{

    Node *curr = head;
    Node *pred = nodeAlloc.top();
    nodeAlloc.pop();
    SuccessValue ret;
    while (true)
    {
        LocatePred(pred, curr, vertex);
        if (!IsNodePresent(curr, vertex))
        {
            ret = UpdateInfo(curr, nDesc, true);
            if (ret != NULL)
            {
                MDList *mdlist = mdlistAlloc.top();
                mdlistAlloc.pop();
                Node *toInsert = nodeAlloc.top();
                nodeAlloc.pop();
                toInsert->set(nDesc, vertex, mdlist, pred->next);
                if (__atomic_compare_exchange_n(&pred->next, &curr, toInsert, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
                    return true;
                else
                    return false;
            }
        }
    }
}

void AdjacencyList::LocatePred(Node *&pred, Node *&curr, uint32_t vertex)
{

    int i = 0;
    while (curr->key < vertex)
    {
        pred = curr;
        curr = curr->next;
    }
}

Node *AdjacencyList::FindVertex(uint32_t vertex, NodeDesc *nDesc)
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
                ExecuteOperations(cDesc, cDesc->desc->currentOp + 1);
            }
            if (IsKeyPresent(cDesc, nDesc->desc))
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

bool AdjacencyList::InsertEdge(uint32_t vertex, uint32_t edge, NodeDesc *nDesc, uint32_t &currDim, uint32_t predDim, uint32_t k[])
{
    while (true)
    {
        Node *currVertex = FindVertex(vertex, nDesc);
        if (currVertex == NULL)
        {
            return false;
        }
        Node *pred = nodeAlloc.top();
        nodeAlloc.pop();
        MDListNode *currEdge = currVertex->list->head;
        while (true)
        {
            MDListNode *pred = mdlNodeAlloc.top();
            mdlNodeAlloc.pop();
            currVertex->list->LocatePred(currVertex->list->head, pred, currDim, predDim, k);
            if (IsNodePresent(currVertex, edge))
            {
                return false;
            }
            else
            {
                MDListNode *n = new MDListNode();
                n->info = nDesc;
                MDList currVertexList = *currVertex->list;
                return currVertexList.Insert(n, currEdge, pred, currDim, predDim);
            }
        }
    }
}

bool AdjacencyList::DeleteEdge(uint32_t vertex, uint32_t edge, NodeDesc *nDesc, uint32_t &currDim, uint32_t predDim, uint32_t k[])
{
    while (true)
    {
        Node *currVertex = FindVertex(vertex, nDesc);
        if (currVertex == NULL)
        {
            return false;
        }
        Node *pred = nodeAlloc.top();
        nodeAlloc.pop();
        while (true)
        {
            MDListNode *pred = mdlNodeAlloc.top();
            mdlNodeAlloc.pop();
            currVertex->list->LocatePred(currVertex->list->head, pred, currDim, predDim, k);
            if (IsNodePresent(currVertex, edge))
            {
                return UpdateInfo(currVertex, nDesc, true) == Success;
            }
            else
            {
                return false;
            }
        }
    }
}

void AdjacencyList::MarkForDelete(Desc *d, std::unordered_set<NodeDesc *> toDelete)
{
    for (const auto &del : toDelete)
    {
        if (del == NULL)
        {
            continue;
        }
    }
}