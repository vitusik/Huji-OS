/**
 * @file Test1.cpp
 * @author Itai Tagar <itagar>
 *
 * @brief Test for CacheFS.
 */


#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "CacheFS.h"


#define BUFFER_SIZE 100000
#define CHAPTER_1 "/tmp/TheBoyWhoLived"
#define CHAPTER_2 "/tmp/TheVanishingGlass"
#define NOT_TMP "TheBoyWhoLived"
#define ROOT_DIRECTORY "/"
#define CACHE_LOG_PATH "CacheLog1"
#define STAT_LOG_PATH "StatLog1"
#define BUFFER_OUTPUT "BufferOutput1"


int main(int argc, char *argv[])
{
    try
    {
        char *buf = nullptr;
        buf = new char[BUFFER_SIZE];
        std::ofstream bufferOutput(BUFFER_OUTPUT);

        // Start LRU Library.
        if (CacheFS_init(4, LRU, 2, 0.5))
        {
            std::cerr << "Error: You should not care about f_old/f_new in LRU" << std::endl;
            return -1;
        }

        // Open the same file twice.
        int f1 = CacheFS_open(CHAPTER_1);
        int f2 = CacheFS_open(CHAPTER_1);
        if (f1 < 0 || f2 < 0)
        {
            std::cerr << "Error: You have a problem in opening legal files" << std::endl;
            return -1;
        }
        // Open file that not in /tmp.
        int f3 = CacheFS_open(NOT_TMP);
        int f4 = CacheFS_open(ROOT_DIRECTORY);
        if (f3 >= 0 || f4 >= 0)
        {
            std::cerr << "Error: You should not support files that are not in /tmp" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        // Read from the second file the first block.
        if (CacheFS_pread(f2, buf, 150, 0) != 150)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        // Close the second file.
        if (CacheFS_close(f2) != 0)
        {
            std::cerr << "Error in CacheFS_close while closing legal file" << std::endl;
            return -1;
        }
        // Close an already closed file.
        if (CacheFS_close(f2) != -1)
        {
            std::cerr << "Error in CacheFS_close while closing illegal file" << std::endl;
            return -1;
        }

        // Read from the first file the first + second block.
        // There should be cache hit for the first block and cache miss for the second.
        if (CacheFS_pread(f1, buf + 150, 4000, 150) != 4000)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        // Read from illegal file.
        if (CacheFS_pread(f3, buf + 4150, 5000, 0) != -1)
        {
            std::cerr << "Error in CacheFS_pread while illegally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        // Read from illegal file.
        if (CacheFS_pread(f2, buf + 4150, 5000, 0) != -1)
        {
            std::cerr << "Error in CacheFS_pread while illegally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        // Read from the first file chunk that is bigger then the file itself.
        if (CacheFS_pread(f1, buf + 4150, 7000, 20000) != 5832)
        {
            std::cerr << "Error in CacheFS_pread while reading with request that is larger then the file size" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        // Read with illegal offset.
        if (CacheFS_pread(f1, buf + 9982, 150, 50000) != 0)
        {
            std::cerr << "Error in CacheFS_pread while reading with large offset" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        // Read from the empty count.
        if (CacheFS_pread(f1, buf + 9982, 0, 100) != 0)
        {
            std::cerr << "Error in CacheFS_pread while reading with zero count" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        // Read from the first file the first + second block.
        if (CacheFS_pread(f1, buf, 7000, 0) != 7000)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        // Read from the first file the sixth block.
        // Should be cache hit.
        if (CacheFS_pread(f1, buf, 5, 24900) != 5)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        // Read from the first file the second + third block.
        if (CacheFS_pread(f1, buf + 5, 8192, 4096) != 8192)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        // Read from the first file the first + second + third block.
        // There all should be cache hits.
        if (CacheFS_pread(f1, buf + 8197, 8192, 150) != 8192)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        // Read from the first file the first block.
        // There should be cache hit.
        if (CacheFS_pread(f1, buf + 16389, 50, 0) != 50)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        CacheFS_destroy();
        bufferOutput << buf;
        bufferOutput.close();
        delete[] buf;

        return 0;

    }
    catch (std::bad_alloc &exception)
    {
        std::cerr << "Exception: " << exception.what() << std::endl;
    }
}




