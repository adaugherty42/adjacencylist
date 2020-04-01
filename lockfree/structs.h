#include "adjacencylist.h"
#include "mdlist.h"

MDListNode::MDListNode()
{
    key = -1;
    aDesc = new AdoptDesc;
    info = new NodeDesc;
}

void MDListNode::set(uint32_t k, AdoptDesc* a, NodeDesc* i)
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
    headVertex = new Node();
    basis = -1;
}

void MDList::set(MDListNode* h, Node* hv, uint32_t b)
{
    head = h;
    headVertex = hv;
    basis = b;
}

Node::Node()
{
    info = new NodeDesc();
    key = -1;
    list = new MDList();
    next = new Node();
}

void Node::set(NodeDesc* i, uint32_t k, MDList* l, Node* n)
{
    info = i;
    key = k;
    list = l;
    next = n;
}



