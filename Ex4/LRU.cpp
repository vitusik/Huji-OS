#include "LRU.h"

LRU_ALG::LRU_ALG(){}

int LRU_ALG::update_block(int index, cache_vec &v) const  {
    Block tmp = v[index];
    v.erase(v.begin() + index);
    try
    {
        v.insert(v.begin(), tmp);
        return 1;
    }
    catch (std::bad_alloc &ba) {
        return -1;
    }
}
