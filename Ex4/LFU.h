#ifndef LFU_H
#define LFU_H
#include "Cache_Manage_Alg.h"

class LFU_ALG : public C_M_A {
public:
    LFU_ALG();
    int update_block(int index, cache_vec &v) const ;
};
#endif //LFU_H
