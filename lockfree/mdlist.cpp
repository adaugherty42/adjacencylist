#include "mdlist.h"
#include <cmath>

std::vector<uint32_t> MDList::keyToCoord(uint32_t key)
{
    std::vector<uint32_t> *k = new std::vector<uint32_t>(DIM);
    uint32_t basis = (uint32_t)ceil(std::pow(N, 1.0 / DIM));
    uint32_t quotient = key;

    for (int i = DIM - 1; i >= 0; i--)
    {
        k->at(i) = (quotient % basis);
        quotient = quotient / basis;
    }

    return *k;
}