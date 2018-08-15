//
// Created by vitusik on 5/29/17.
//
#include "CacheFS.h"
#include "Cache_Manage_Alg.h"
#include "FBR.h"
#include "LFU.h"
#include "LRU.h"
#include <math.h>
#include <climits>
#include <fcntl.h>
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>

#define FAIL_VAL -1
#define SUCCESS 0
#define REFERENCE_COUNT_INIT 1
C_M_A* alg;
int unsigned cache_len, hits, misses;
size_t os_block_size;

typedef std::pair<int, std::string> fd_path_pair;
typedef std::vector<fd_path_pair> fd_path_vec;



cache_vec cache;
fd_path_vec fd_path;
std::map<int, int> fd_open_calls;

int CacheFS_init(int blocks_num, cache_algo_t cache_algo, double f_old, double f_new) {
    if(blocks_num <= 0)
    {
        return FAIL_VAL;
    }
    if(cache_algo == FBR && (f_old + f_new > 1 || f_old < 0 || f_new < 0))
    {
        return FAIL_VAL;
    }


    int length_of_new = (int)floor(blocks_num * f_new) - 1;
    int length_of_old = (int)floor(blocks_num * f_old) - 1;
    cache_len = blocks_num;
    LRU_ALG *lru = new LRU_ALG();
    LFU_ALG *lfu = new LFU_ALG();
    FBR_ALG *fbr = new FBR_ALG(length_of_new, length_of_old);
    try
    {
        switch (cache_algo)
        {
            case LRU:
                alg = (C_M_A*)lru;
                delete lfu;
                delete fbr;
                break;
            case LFU:
                alg = (C_M_A*)lfu;
                delete lru;
                delete fbr;
                break;
            case FBR:
                delete lfu;
                delete lru;
                alg = (C_M_A*)fbr;
                break;
        }
    }
    catch (std::bad_alloc &ba)
    {
        return FAIL_VAL;
    }

    struct stat fi;
    int ret = stat("/tmp", &fi);
    if(ret == FAIL_VAL)
    {
        return FAIL_VAL;
    }
    os_block_size = (size_t)fi.st_blksize;
    hits = 0, misses = 0;
    return SUCCESS;
}

int CacheFS_destroy() {
    delete alg;
    return 0;
}

int CacheFS_open(const char *pathname) {
    char full_path_buff[PATH_MAX];
    char* ptr;
    ptr = realpath(pathname, full_path_buff);
    if(ptr == NULL)
    {
        return FAIL_VAL;
    }
    std::string full_path_string(full_path_buff);
    int fd = FAIL_VAL;
    fd_path_vec::iterator it;
    bool found_file = false;
    for(it = fd_path.begin(); it != fd_path.end(); it++)
    {
        if((*it).second == full_path_string)
        {
            // the file has been open before, thus we increment amount of time that
            // file has been opened
            fd = (*it).first;
            (*(fd_open_calls.find(fd))).second ++;
            found_file = true;
            break;
        }
    }
    if(!found_file)
    {
        // file hasn't been open before
        fd = open(full_path_buff,O_RDONLY | O_DIRECT | O_SYNC);
        if(fd == FAIL_VAL)
        {
            return FAIL_VAL;
        }
        else
        {
            fd_path.push_back(fd_path_pair(fd, full_path_string));
            fd_open_calls[fd] = REFERENCE_COUNT_INIT;
        }
    }
    return fd;
}

int CacheFS_close(int file_id) {
    int fd = (*(fd_open_calls.find(file_id))).second;
    int ret_val = 0;
    if(fd == 0)
    {
        ret_val = close(file_id);
    }
    else
    {
        (*(fd_open_calls.find(file_id))).second --;
    }
    return ret_val;
}

int CacheFS_pread(int file_id, void *buf, size_t count, off_t offset) {
    fd_path_vec::iterator fd_path_it;
    std::string path = "";
    int amount_read = 0;
    int left_to_read = count;
    int fd = (*(fd_open_calls.find(file_id))).second;
    if(fd == 0 || fd == FAIL_VAL )
    {
        // the file is closed, therefore we cant read from it, or we got an error
        return FAIL_VAL;
    }
    for(fd_path_it = fd_path.begin(); fd_path_it != fd_path.end(); fd_path_it++)
    {
        if((*fd_path_it).first == file_id)
        {
            path = (*fd_path_it).second;
            break;
        }
    }
    int start_block = (int)(offset/os_block_size);
    int last_block = (int)((count + offset)/os_block_size);
    size_t test = offset % os_block_size;
    for(int cur_block = start_block; cur_block <= last_block; cur_block++)
    {
        bool cache_hit = false;
        for(int unsigned j = 0; j < cache.size(); j++)
        {
            if(cache[j].getPath() == path && cache[j].getBlock_num_inside_file() == cur_block)
            {
                memcpy(buf, (char*)cache[j].getData() + test, left_to_read);
                amount_read += cache[j].getLen_of_data();
                alg->update_block(j, cache);
                hits ++;
                cache_hit = true;
                break;
            }
        }
        if(!cache_hit)
        {
            if(cache.size() == cache_len)
            {
                alg->remove_block(cache);
            }

            misses ++;
            void* new_buff = aligned_alloc(os_block_size, os_block_size);
            if(new_buff == NULL)
            {
                return FAIL_VAL;
            }
            Block *new_block = new (std::nothrow)Block(os_block_size, path, cur_block);
            if(new_block == nullptr)
            {
                return FAIL_VAL;
            }
            ssize_t cur_amount_read = pread(file_id, (*new_block).getData(), os_block_size,
                                            os_block_size * cur_block);
            if(cur_amount_read == FAIL_VAL)
            {
                return FAIL_VAL;
            }
            amount_read += cur_amount_read;
            (*new_block).setLen_of_data((size_t)cur_amount_read);
            alg->add_block(*new_block, cache);
            alg->update_block(0, cache);
            memcpy(buf, (char*)cache[0].getData() + test, left_to_read);
            //free(new_buff);
        }
    }
    left_to_read -= os_block_size;
    return amount_read;
}

int CacheFS_print_cache(const char *log_path) {
    try
    {
        cache_vec::iterator it;
        std::ofstream logfile;
        logfile.open(log_path, std::ios::app);
        for(it = cache.begin(); it != cache.end(); it ++)
        {
            logfile<< (*it).getPath()<<" "<< (*it).getBlock_num_inside_file()<<std::endl;
        }
        logfile.close();
        return 0;
    }
    catch(...)
    {
        return FAIL_VAL;
    }
}

int CacheFS_print_stat(const char *log_path) {
    try
    {
        std::ofstream logfile;
        logfile.open(log_path, std::ios::app);
        logfile<<"Hits number: "<< hits <<std::endl<<"Misses number: "<< misses<<std::endl;
        logfile.close();
        return 0;
    }
    catch (...)
    {
        return FAIL_VAL;
    }
}
