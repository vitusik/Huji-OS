#ifndef CACHE_MANAGE_ALG_H
#define CACHE_MANAGE_ALG_H

#include "Block.h"
#include <new>

struct C_M_A{
public:
    virtual ~C_M_A(){}
    virtual int add_block(Block block, cache_vec &v)
    {
        try
        {
            v.insert(v.begin(), block);
            return 1;
        }
        catch(std::bad_alloc& ba)
        {
            return -1;
        }
    };
    virtual int update_block(int index, cache_vec &v) const = 0;
    virtual void remove_block(cache_vec &v)
    {
        v.pop_back();
    }
};

#endif //CACHE_MANAGE_ALG_H
