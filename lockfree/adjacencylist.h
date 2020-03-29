#pragma once

#include <stdint.h>
#include "mdlist.h"

// LFTT Definitions
enum TxStatus
{
    ACTIVE,
    COMMITTED,
    ABORTED
};

enum OpType
{
    InsertVertex,
    DeleteVertex,
    InsertEdge,
    DeleteEdge,
    Find
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