#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>
#include <cstring>

class Block {
private:
    int times_accessed;
    int block_num_inside_file;
    std::string path;
    void* data;
    size_t len_of_data;
    int ref_cout;

public:
    Block(size_t block_size, std::string file_path, int block_num);
    ~Block();
    int getTimes_accessed() const;

    int getBlock_num_inside_file() const;

    const std::string &getPath() const;

    int getRef_cout() const;

    void inc_time_accessed();

    void inc_ref_cout();

    void *getData() const;

    size_t getLen_of_data() const;

    void setLen_of_data(size_t len_of_data);
};

typedef std::vector<Block> cache_vec;
#endif //BLOCK_H
