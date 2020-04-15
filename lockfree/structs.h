#pragma once
#include "lfstructs.h"
#include "adjacencylist.h"
#include <stack>

inline std::stack<Node *> nodeAlloc;
inline std::stack<Desc *> descAlloc;
inline std::stack<NodeDesc *> nDescAlloc;
inline std::stack<MDList *> mdlistAlloc;
inline std::stack<MDListNode *> mdlNodeAlloc;
inline std::stack<AdoptDesc *> aDescAlloc;

inline MDListNode::MDListNode()
{
    key = -1;
    aDesc = new AdoptDesc();
    info = new NodeDesc();
}

inline void MDListNode::set(uint32_t k, AdoptDesc *a, NodeDesc *i)
{
    key = k;
    aDesc = a;
    info = i;
}

inline AdoptDesc::AdoptDesc()
{
    curr = new MDListNode();
    dp = -1;
    dc = -1;
}

inline void AdoptDesc::set(MDListNode *cur, uint32_t p, uint32_t c)
{
    curr = cur;
    dp = p;
    dc = c;
}

inline Desc::Desc()
{
    size = -1;
    currentOp = -1;
}

inline void Desc::set(uint32_t s, TxStatus ts, int c)
{
    size = s;
    status = ts;
    currentOp = c;
}

inline NodeDesc::NodeDesc()
{
    desc = new Desc();
    opid = -1;
}

inline void NodeDesc::set(Desc *d, uint32_t opn)
{
    desc = d;
    opid = opn;
}

inline MDList::MDList()
{
    head = new MDListNode();
    basis = -1;
}

inline void MDList::set(MDListNode *h, uint32_t b)
{
    head = h;
    basis = b;
}

inline Node::Node()
{
    info = new NodeDesc();
    key = -1;
    list = new MDList();
    next = new Node();
}

inline void Node::set(NodeDesc *i, uint32_t k, MDList *l, Node *n)
{
    info = i;
    key = k;
    list = l;
    next = n;
}

inline void Operation::set(OpType t, uint32_t k)
{
    type = t;
    key = k;
}