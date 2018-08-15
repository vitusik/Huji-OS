#include <iostream>
#include <stdlib.h>
#include "MapReduceClient.h"
#include "MapReduceFramework.h"

pthread_mutex_t k2_mutex = PTHREAD_MUTEX_INITIALIZER;

std::vector<k2Base *> k2;

pthread_mutex_t v2_mutex = PTHREAD_MUTEX_INITIALIZER;

std::vector<v2Base *> v2;

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
            pthread_mutex_lock(&k2_mutex);
            pthread_mutex_lock(&v2_mutex);
            k2.push_back(new elements(0));
            v2.push_back(new elements(1));
            Emit2(k2[k2.size() -1],v2[v2.size() -1]);
            pthread_mutex_unlock(&k2_mutex);
            pthread_mutex_unlock(&v2_mutex);
        }
        else
        {
            pthread_mutex_lock(&k2_mutex);
            pthread_mutex_lock(&v2_mutex);
            k2.push_back(new elements(1));
            v2.push_back(new elements(1));
            Emit2(k2[k2.size() -1],v2[v2.size() -1]);
            pthread_mutex_unlock(&k2_mutex);
            pthread_mutex_unlock(&v2_mutex);
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
    for (int j = 0; j < 1000; ++j)
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
    OUT_ITEMS_VEC result = RunMapReduceFramework(s, in_items_vec, 10, false);

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
    for (int k = 0; k < k2.size(); ++k) {
        delete k2[k];
    }
    for (int l = 0; l < v2.size(); ++l) {
        delete v2[l];
    }
    delete result[0].first;
    delete result[0].second;
    delete result[1].first;
    delete result[1].second;
    return 0;
}