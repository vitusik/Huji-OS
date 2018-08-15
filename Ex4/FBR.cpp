#include "FBR.h"

FBR_ALG::FBR_ALG(const int i, const int j) : new_end_block_index(i), old_start_block_index(j) {}

int FBR_ALG::update_block(int index, cache_vec &v) const  {
    Block tmp = v[index];
    if(index > new_end_block_index)
    {
        tmp.inc_ref_cout();
    }
    v.erase(v.begin() + index);
    try
    {
        v.insert(v.begin(), tmp);
        return 1;
    }
    catch(std::bad_alloc& ba)
    {
        return -1;
    }
}

void FBR_ALG::remove_block(cache_vec &v)  {
    Block min = v[old_start_block_index];
    int min_index = old_start_block_index;
    for(int unsigned i = old_start_block_index + 1; i < v.size(); i++)
    {
        if (min.getRef_cout() >= v[i].getRef_cout())
        {
            min_index = i;
            min = v[i];
        }
    }
    v.erase(v.begin() + min_index);
}
