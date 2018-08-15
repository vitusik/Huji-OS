
#include "Block.h"

Block::Block(size_t block_size, std::string file_path, int block_num)
{
    data = aligned_alloc(block_size, block_size);
    path = file_path;
    ref_cout = 1;
    block_num_inside_file = block_num;
    times_accessed = 0;
}

Block::~Block() {}

int Block::getTimes_accessed() const {
    return times_accessed;
}

int Block::getBlock_num_inside_file() const {
    return block_num_inside_file;
}

const std::string &Block::getPath() const {
    return path;
}

int Block::getRef_cout() const {
    return ref_cout;
}

void Block::inc_time_accessed() {
    times_accessed ++;
}

void Block::inc_ref_cout() {
    ref_cout ++;
}

void *Block::getData() const {
    return data;
}

size_t Block::getLen_of_data() const {
    return len_of_data;
}

void Block::setLen_of_data(size_t len_of_data) {
    Block::len_of_data = len_of_data;
}
