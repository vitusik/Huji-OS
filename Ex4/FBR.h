#ifndef FBR_H
#define FBR_H

#include "Cache_Manage_Alg.h"

class FBR_ALG: public C_M_A
{
public:
    FBR_ALG(const int i, const int j);
    int update_block(int index, cache_vec &v) const override;
    void remove_block(cache_vec &v) override;
private:

    int new_end_block_index;
    int old_start_block_index;
};

#endif //FBR_H
