#pragma once
#include "lfstructs.h"
#include "adjacencylist.h"
#include <stack>

std::stack<Node *> nodeAlloc;
std::stack<Desc *> descAlloc;
std::stack<NodeDesc *> nDescAlloc;
std::stack<MDList *> mdlistAlloc;
std::stack<MDListNode *> mdlNodeAlloc;
std::stack<AdoptDesc *> aDescAlloc;

MDListNode::MDListNode()
{
    key = -1;
    aDesc = new AdoptDesc;
    info = new NodeDesc;
}

void MDListNode::set(uint32_t k, AdoptDesc *a, NodeDesc *i)
{
    key = k;
    aDesc = a;
    info = i;
}

AdoptDesc::AdoptDesc()
{
    curr = new MDListNode;
    dp = -1;
    dc = -1;
}

void AdoptDesc::set(MDListNode *cur, uint32_t p, uint32_t c)
{
    curr = cur;
    dp = p;
    dc = c;
}

Desc::Desc()
{
    size = -1;
    currentOp = -1;
}

void Desc::set(uint32_t s, TxStatus ts, int c)
{
    size = s;
    status = ts;
    currentOp = c;
}

NodeDesc::NodeDesc()
{
    desc = new Desc();
    opid = -1;
}

void NodeDesc::set(Desc *d, uint32_t opn)
{
    desc = d;
    opid = opn;
}

MDList::MDList()
{
    head = new MDListNode();
    basis = -1;
}

void MDList::set(MDListNode *h, uint32_t b)
{
    head = h;
    basis = b;
}

Node::Node()
{
    info = new NodeDesc();
    key = -1;
    list = new MDList();
    next = new Node();
}

void Node::set(NodeDesc *i, uint32_t k, MDList *l, Node *n)
{
    info = i;
    key = k;
    list = l;
    next = n;
}

void Operation::set(OpType t, uint32_t k)
{
    type = t;
    key = k;
}