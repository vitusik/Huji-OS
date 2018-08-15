#include "MapReduceClient.h"
#include "MapReduceFramework.h"
#include <stdlib.h>
#include <iostream>

#define N 4000
#define RANGE 10
#define THREADS 4

using namespace std;
struct Number : public k1Base, public k2Base, public k3Base, public v1Base, public v2Base, public v3Base {

    int n;

    bool operator<(const k1Base &other) const {
        return n < ((Number&)other).n;
    }
    bool operator<(const k2Base &other) const {
        return n < ((Number&)other).n;
    }
    bool operator<(const k3Base &other) const {
        return n < ((Number&)other).n;
    }

    ~Number() {

    }
};

struct MRNumber : public MapReduceBase
{
    void Map(const k1Base *const key, const v1Base *const val) const override {

        if(isPrime(*(Number*)key)) {
            auto k = new Number();
            k->n = ((Number*)key)->n;
            auto v = new Number();
            v->n = 1;
            Emit2(k, v);
        }
    }

    void Reduce(const k2Base *const key, const V2_VEC &vals) const override {
        auto k = (Number*)key;
        int count=0;
        for(v2Base* val: vals){
            (void)val;
            count++;
        }

        k3Base * k3 = new Number();
        v3Base * v3 = new Number();
        ((Number*)k3)->n=k->n;
        ((Number*)v3)->n=count;
        Emit3(k3, v3);
    }

    bool isPrime(Number &n) const {
        for (int i = 2; i < n.n; ++i) {
            if(n.n % i == 0) {
                return false;
            }
        }
        return true;
    }
};

int main()
{
    IN_ITEMS_VEC numbers;
    MRNumber m;
    srand(time(NULL));
    for (int i = 0; i < N; ++i) {
        int n = std::rand() % RANGE + 1;
        std::cout << n << endl;
        auto numKey = new Number();
        auto numVal= new Number();
        numKey->n = n;
        numVal->n = 1;
        numbers.push_back(make_pair(numKey, numVal));
    }
    auto results = RunMapReduceFramework(m, numbers, THREADS, true);

    int sum = 0;
    for(auto &i : results) {
        std::cout << ((Number*)i.first)->n << " " << ((Number*)i.second)->n << endl;
        sum+=((Number*)i.second)->n;
     }

    std::cout << sum << endl;
    for(auto &i : results) {
        delete i.first;
        delete i.second;
    }

    for (int j = 0; j < N; ++j)
    {
        delete numbers[j].first;
        delete numbers[j].second;
    }

    return 0;
}