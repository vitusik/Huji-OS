//
// Created by Vitaly Frolov on 5/25/17.
//

#include <string>
#include <iostream>
#include "MapReduceClient.h"
#include "MapReduceFramework.h"
#include <sys/stat.h>
#include <dirent.h>

#define  NUMBER_OF_THREADS 4
std::string substring_to_search;


struct File: public k1Base, public k2Base, public k3Base
{
public:
    File(const std::string &file) : file_name(file)
    {}
    bool operator<(const k3Base &other) const override {
        return this->file_name < ((File &) other).file_name;
    }
    bool operator<(const k2Base &other) const override {
        return this->file_name < ((File &) other).file_name;
    }
    bool operator<(const k1Base &other) const override {
        return this->file_name < ((File &) other).file_name;
    }
    std::string get_file_name()
    {
        return file_name;
    }

    ~File() override
    {}
private:
    const std::string file_name;
};

class Amount: public v1Base, public v2Base, public v3Base
{
public:
    Amount(const int x) : amount(x)
    {}
    int get_amount()
    {
        return amount;
    }
private:
    int amount;
};

struct MRB : public MapReduceBase
{
    void Map(const k1Base *const key, const v1Base *const val) const override
    {
        std::string cur_file = ((File*)key)->get_file_name();
        std::size_t found = cur_file.find(substring_to_search);
        if(found != std::string::npos)
        {
            File *f = new File(cur_file);
            Amount *a = new Amount(1);
            Emit2(f, a);
        }

    }

    void Reduce(const k2Base *const key, const V2_VEC &vals) const override
    {
        int sum = 0;
        for(auto &i : vals)
        {
            sum += ((Amount*)i)->get_amount();
        }
        std::string cur_file = ((File*)key)->get_file_name();
        File *f = new File(cur_file);
        Amount *a = new Amount(sum);
        Emit3(f, a);
    }
};

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        std::cout<<"Usage: <substring to search> <folders, separated by space>";
        exit(0);
    }
    substring_to_search = argv[1];
    IN_ITEMS_VEC v;
    for(int i = 2; i < argc; i++)
    {
        std::string cur = argv[i];
        DIR *dp;
        struct dirent *dirp;
        if((dp  = opendir(cur.c_str())) == NULL) {
            std::cout << "Error(" << errno << ") opening " << cur << std::endl;
        }

        while ((dirp = readdir(dp)) != NULL) {
            File *f = new File(std::string(dirp->d_name));
            Amount *a = new Amount(1);
            v.push_back(std::make_pair(f,a));
        }
        closedir(dp);       
    }
    MRB m;
    OUT_ITEMS_VEC result = RunMapReduceFramework(m, v,NUMBER_OF_THREADS, true);
    for(auto &i : result)
    {
        for(int j= 0; j < ((Amount*)i.second)->get_amount(); j++)
        {
            std::cout<<((File*)i.first)->get_file_name()<<" ";
        }
    }

    for(auto &i:v)
    {
        delete i.first;
        delete i.second;
    }
    for(auto &i:result)
    {
        delete i.first;
        delete i.second;
    }
}
