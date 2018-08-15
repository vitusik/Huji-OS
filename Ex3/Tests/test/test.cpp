#include <iostream>
#include "MapReduceClient.h"
#include "MapReduceFramework.h"

#define NUM_ITERS 10000

#define NUM_PER_ITER 500

struct NumWrapper : public k1Base, public v1Base, public k2Base, public v2Base, public k3Base, public v3Base
{
    int num;

    bool operator<(const k1Base &other) const override
    {
        return num < ((NumWrapper &) other).num;
    }

    bool operator<(const k2Base &other) const override
    {
        return num < ((NumWrapper &) other).num;
    }

    bool operator<(const k3Base &other) const override
    {
        return num < ((NumWrapper &) other).num;
    }

    ~NumWrapper()
    {

    }
};

struct MapReduceImp : public MapReduceBase
{
    void Map(const k1Base *const key, const v1Base *const val) const override
    {
        auto s = (NumWrapper *) key;
        auto e = (NumWrapper *) val;

        for (int i = s->num; i < e->num; ++i)
        {
            auto one = new NumWrapper();
            one->num = 1;
            auto k = new NumWrapper();
            k->num = i;
            Emit2(k, one);
        }
    }

    void Reduce(const k2Base *const key, const V2_VEC &vals) const override
    {
        auto n = (NumWrapper *const) key;
        int r = 0;
        for (auto &i : vals)
        {
            r++;
        }
        auto res = new NumWrapper();
        res->num = n->num;
        auto times = new NumWrapper();
        times->num = r;
        Emit3(res, times);
    }
};

int main(int argc, char *argv[])
{
    IN_ITEMS_VEC v;
    for (int i = 0; i < NUM_ITERS; ++i)
    {
        auto s = new NumWrapper();
        s->num = 1;
        auto e = new NumWrapper();
        e->num = NUM_PER_ITER;
        auto p = std::make_pair(s, e);
        v.push_back(p);
    }
    MapReduceImp m;
    auto results = RunMapReduceFramework(m, v, 4, true);
    for (auto &item : results)
    {
        std::cout << ((NumWrapper *) item.first)->num << " : " << ((NumWrapper *) item.second)->num << " times"
                  << std::endl;
        delete item.first;
        delete item.second;
    }
    for (int i = 0; i < NUM_ITERS; ++i)
    {
        delete v.at(i).first;
        delete v.at(i).second;
    }
}