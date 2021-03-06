#include "structs.h"
#include <cmath>

// Given a key, find its coordinate vector. Using a 32 bit keyspace and 8 dimensions
// each coordinate is of base 16.
uint32_t *MDList::KeyToCoord(uint32_t key)
{
    uint32_t *k = new uint32_t[DIM];
    uint32_t basis = (uint32_t)ceil(std::pow(N, 1.0 / DIM));
    uint32_t quotient = key;

    for (int i = DIM - 1; i >= 0; i--)
    {
        k[i] = (quotient % basis);
        quotient = quotient / basis;
    }

    return k;
}

// We want to pass in references to the current/pred dimensions, so that these values are also updated
// in the calling function.
inline void MDList::LocatePred(MDListNode *&curr, MDListNode *&pred, uint32_t &dc, uint32_t &dp, uint32_t k[])
{
    while (dc < DIM)
    {
        while (curr != NULL && k[dc] > curr->k[dc])
        {
            pred = curr;
            dp = dc;
            AdoptDesc *ad = curr->aDesc;

            if (ad != NULL && (dp == ad->dp || dp == ad->dc))
            {
                //FinishInserting(curr, ad);
            }
            curr = (MDListNode *)ClearMark(curr->child[dc], F_all);
        }

        if (curr == NULL && k[dc] < curr->k[dc])
        {
            break;
        }
        // keep going
        else
        {
            dc++;
        }
    }
}

// Attempt to find a matching key within the MDList
bool MDList::Find(uint32_t key)
{
    MDListNode *curr = head;
    MDListNode *pred = mdlNodeAlloc.top();
    mdlNodeAlloc.pop();
    uint32_t dc, dp = 0;
    LocatePred(pred, curr, dc, dp, KeyToCoord(key));
    return dc == DIM;
}

// Since we're using LFTT, we don't need to call LocatePred...it's given to us by the calling
// adjacency list method. We effectively start on line 7 of MDList algorithm 7 instead of line 1.
bool MDList::Delete(MDListNode *&curr, MDListNode *&pred, uint32_t dc, uint32_t dp)
{
    if (dc != DIM)
    {
        return false;
    }
    MDListNode *marked = (MDListNode *)SetMark(curr, F_del);

    MDListNode *child = __sync_val_compare_and_swap(&pred->child[dp], curr, marked);

    if ((MDListNode *)ClearMark(child, F_all) == curr)
    {
        if (!IsMarked(child, F_all))
        {
            return true;
        }
        else if (IsMarked(child, F_del))
        {
            return false;
        }
    }
    return false;
}

// Some of the initial work is handled by the method that calls this one, so we effectively start on
// line 13 of MDList algorithm 5
bool MDList::Insert(MDListNode *&node, MDListNode *&curr, MDListNode *&pred, uint32_t &dc, uint32_t &dp)
{
    AdoptDesc *ad = curr != NULL ? curr->aDesc : NULL;
    if (ad != NULL && dp != dc)
    {
        // Another thread is already inserting, so let's help them out
        FinishInserting(curr, ad);
    }
    if (IsMarked(pred->child[dp], F_del))
    {
        curr = (MDListNode *)SetMark(curr, F_del);
        if (dc == DIM - 1)
        {
            dc = DIM;
        }
    }
    FillNewNode(node, curr, pred, dc, dp);
    // Try to CAS our node in
    if (__atomic_compare_exchange_n(&pred->child[dp], &curr, node, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
    {
        if (ad != NULL)
        {
            // Help the other thread finish its insertion
            FinishInserting(node, ad);
        }
        return true;
    }
    return false;
}

void MDList::FinishInserting(MDListNode *n, AdoptDesc *ad)
{
    MDListNode *child;
    MDListNode *curr = ad->curr;
    int dp = ad->dp;
    int dc = ad->dc;

    // i in range [dp,dc)
    for (int i = dp; i < dc; i++)
    {
        child = curr->child[i];
        // call the builtin FetchAndOr method to set the flag on the child pointers
        child = (MDListNode*)__sync_fetch_and_or((uintptr_t**)curr->child[i], (uintptr_t*)F_adp);
        // clear our mark
        child = (MDListNode *)ClearMark(child, F_adp);
        if (n->child[i] == NULL)
        {
            __atomic_compare_exchange_n(&n->child[i], NULL, child, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
        }
    }
    __atomic_compare_exchange_n(&n->aDesc, &ad, NULL, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
}

// Helper method used by Insert
void MDList::FillNewNode(MDListNode *&node, MDListNode *&curr, MDListNode *&prev, uint32_t &dc, uint32_t &dp)
{
    AdoptDesc *ad = aDescAlloc.top();
    aDescAlloc.pop();
    if (dp != dc)
    {
        ad->set(curr, dp, dc);
    }
    for (int i = 0; i <= dp; i++)
    {
        node->child[i] = (MDListNode *)F_adp;
    }
    for (int i = dp; i <= DIM; i++)
    {
        node->child[i] = NULL;
    }
    if (dc < DIM)
    {
        node->child[dc] = curr;
    }
    node->aDesc = ad;
}