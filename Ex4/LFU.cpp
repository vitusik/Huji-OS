#include "LFU.h"

LFU_ALG::LFU_ALG() {}

int LFU_ALG::update_block(int index, cache_vec &v) const  {
    Block tmp = v[index];
    v.erase(v.begin() + index);
    tmp.inc_time_accessed();
    cache_vec::iterator it;
    for(it = v.begin(); it != v.end(); it++)
    {
        if((*it).getTimes_accessed() <= tmp.getTimes_accessed())
        {
            try {
                v.insert(it, tmp);
                return 1;
            }
            catch(std::bad_alloc& ba)
            {
                return -1;
            }
        }
    }
    return -1; // never gets here
}
