#include <iostream>
#include "MapReduceClient.h"
#include "MapReduceFramework.h"

class elements : public k1Base,public k2Base,public k3Base,public v1Base,public v2Base,public v3Base
{
public:

    bool operator<(const k1Base &other) const
    {
        return true;
    }

    bool operator<(const k2Base &other) const
    {
        return true;
    }

    bool operator<(const k3Base &other) const
    {
        return true;
    }

    bool operator<(const v1Base &other) const
    {
        return true;
    }

    bool operator<(const v2Base &other) const
    {
        return true;
    }

    bool operator<(const v3Base &other) const
    {
        return true;
    }
};

class tester : public MapReduceBase
{
public:

    virtual void Map(const k1Base *const key, const v1Base *const val) const
    {
    }

    virtual void Reduce(const k2Base *const key, const V2_VEC &vals) const
    {
        std::cout<<"this should not be printed"<<std::endl;
    }
};

/**
 * @brief The main function running the program.
 */
int main(int argc, char *argv[])
{
    tester s;
    IN_ITEMS_VEC in_items_vec;
    OUT_ITEMS_VEC result = RunMapReduceFramework(s, in_items_vec, 10, true);
    for (int i = 0; i < result.size(); ++i)
    {
        std::cout<<"the result from this map reduce should be empty"<<std::endl;
    }
    std::cout<<"if only this was printed great. if some thing else was printed it is a "
            "problem"<<std::endl;
    return 0;
}