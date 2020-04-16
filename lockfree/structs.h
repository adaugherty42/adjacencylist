#pragma once
#include "lfstructs.h"
#include "adjacencylist.h"
#include <stack>
#include <iostream>

inline std::stack<Node *> nodeAlloc;
inline std::stack<Desc *> descAlloc;
inline std::stack<NodeDesc *> nDescAlloc;
inline std::stack<MDList *> mdlistAlloc;
inline std::stack<MDListNode *> mdlNodeAlloc;
inline std::stack<AdoptDesc *> aDescAlloc;

inline MDListNode::MDListNode()
{
//        std::cout << "MDLNode const start \n";

    key = 0;
    aDesc = (AdoptDesc*) malloc(sizeof(AdoptDesc));
    k = (uint32_t*)(sizeof(uint32_t)*DIM);
    child = (MDListNode**) malloc(sizeof(MDListNode*)*DIM);
    info = new NodeDesc();
//        std::cout << "MDLNode const end \n";

}

inline void MDListNode::set(uint32_t k, AdoptDesc *a, NodeDesc *i)
{
    key = k;
    aDesc = a;
    info = i;
}

inline AdoptDesc::AdoptDesc()
{
    //    std::cout << "AdoptDesc const start \n";

    curr = (MDListNode*) malloc(sizeof(MDListNode));
    dp = 0;
    dc = 0;

       // std::cout << "AdoptDesc const start \n";

}

inline void AdoptDesc::set(MDListNode *cur, uint32_t p, uint32_t c)
{
    curr = cur;
    dp = p;
    dc = c;
}

inline Desc::Desc(uint32_t n)
{
       // std::cout << "Desc const start \n";

    size = 0;
    currentOp = 0;
    ops = (Operation*) malloc(sizeof(Operation) * n);
        //std::cout << "Desc const end \n";

}

inline void Desc::set(uint32_t s, TxStatus ts, int c)
{
    size = s;
    status = ts;
    currentOp = c;
}

inline NodeDesc::NodeDesc()
{
       // std::cout << "nodeDesc const start \n";

    desc = new Desc(2);
    opid = 0;
        // std::cout << "nodeDesc const end \n";

}

inline void NodeDesc::set(Desc *d, uint32_t opn)
{
    desc = d;
    opid = opn;
}

inline MDList::MDList()
{
       // std::cout << "MDList const start \n";

    head = new MDListNode();
    basis = -1;
//        std::cout << "MDlist const end \n";

}

inline void MDList::set(MDListNode *h, uint32_t b)
{
    head = h;
    basis = b;
}

inline Node::Node()
{
   //std::cout << "node const start \n";
    info = new NodeDesc();
    key = -1;
    list = new MDList();
    //std::cout << "node const got heres \n";
    next = (Node*) malloc(sizeof(struct Node));
   //std::cout << "node const end \n";


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