#pragma once
#include <stdint.h>
#include <stddef.h>
// LFTT Definitions
enum TxStatus
{
    ACTIVE,
    COMMITTED,
    ABORTED
};

// had to put Op at the end because compiler didn't like DeleteVertex
enum OpType
{
    InsertVertexOp,
    DeleteVertexOp,
    InsertEdgeOp,
    DeleteEdgeOp,
    FindOp
};

// Needed for UpdateInfo
enum SuccessValue
{
    Fail,
    Success,
    Retry
};

struct Operation
{
    OpType type;
    uint32_t key;
    void set(OpType t, uint32_t k);
};

struct Desc
{
    uint32_t size;
    TxStatus status;
    int currentOp;
    Operation ops[];

    Desc();
    void set(uint32_t s, TxStatus ts, int c);
};

struct NodeDesc
{
    Desc *desc;
    uint32_t opid;

    NodeDesc();
    void set(Desc *d, uint32_t opn);
};
