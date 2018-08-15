#include <iostream>
#include "MapReduceClient.h"
#include "MapReduceFramework.h"

class elements : public k1Base,public k2Base,public k3Base,public v1Base,public v2Base,public v3Base
{
public:

    elements(int i){num = i;}

    bool operator<(const k1Base &other) const
    {
        return num < dynamic_cast< const elements &>(other).num;
    }

    bool operator<(const k2Base &other) const
    {
        return num < dynamic_cast< const elements &>(other).num;
    }

    bool operator<(const k3Base &other) const
    {
        return num < dynamic_cast< const elements &>(other).num;
    }

    bool operator<(const v1Base &other) const
    {
        return num < dynamic_cast< const elements &>(other).num;
    }

    bool operator<(const v2Base &other) const
    {
        return num < dynamic_cast< const elements &>(other).num;
    }

    bool operator<(const v3Base &other) const
    {
        return num < dynamic_cast< const elements &>(other).num;
    }

    int num;
};

class tester : public MapReduceBase
{
public:

    virtual void Map(const k1Base *const key, const v1Base *const val) const
    {
        if ((((elements*)key)->num)%2 == 0)
        {
            Emit2(new elements(0),new elements(1));
        }
        else
        {
            Emit2(new elements(1),new elements(1));
        }
        delete key;
        delete val;
    }

    virtual void Reduce(const k2Base *const key, const V2_VEC &vals) const
    {
        Emit3(new elements(((elements*)key)->num),new elements((int)vals.size()));
    }
};

/**
 * @brief The main function running the program.
 */
int main(int argc, char *argv[])
{
    std::srand(100000);
    int i;
    int odd = 0;
    int even = 0;
    tester s;
    IN_ITEMS_VEC in_items_vec;
    for (int j = 0; j < 1000000; ++j)
    {
        i = std::rand();
        if (i%2 == 0)
        {
            even += 1;
        }
        else{
            odd += 1;
        }
        in_items_vec.push_back(std::pair<k1Base *, v1Base *>(new elements(i), NULL));
    }
    OUT_ITEMS_VEC result = RunMapReduceFramework(s, in_items_vec, 10, true);

    if (result.size() != 2)
    {
        std::cout<<"you have to many elements returned in the result"<<std::endl;
        return 0;
    }
    int you_even = ((elements *)result[0].second)->num;
    int you_odd = ((elements *)result[1].second)->num;
    if ( you_even != even ||  you_odd != odd)
    {
        std::cout<<"opps you said "<<you_even<<" and their is "<<even<<" even nums ";
        std::cout<<"and you said "<<you_odd<<" and their is "<<odd<<" odd nums"<<std::endl;
    }
    else
    {
        std::cout<<"good"<<std::endl;
    }
    delete result[0].first;
    delete result[0].second;
    delete result[1].first;
    delete result[1].second;
    return 0;
}