#ifndef LRU_H
#define LRU_H

#include "Cache_Manage_Alg.h"

class LRU_ALG : public C_M_A
{
public:
    LRU_ALG();
    int update_block(int index, cache_vec &v) const ;
};
#endif //LRU_H
