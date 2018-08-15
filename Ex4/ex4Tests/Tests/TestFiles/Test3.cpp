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
#define NOT_TMP "/var/tmp/TheVanishingGlass"
#define CACHE_LOG_PATH "CacheLog3"
#define STAT_LOG_PATH "StatLog3"
#define BUFFER_OUTPUT "BufferOutput3"


int main(int argc, char *argv[])
{
    try
    {
        char *buf = nullptr;
        buf = new char[BUFFER_SIZE];
        std::ofstream bufferOutput(BUFFER_OUTPUT);

        if (!CacheFS_init(10, FBR, 3, 3))
        {
            std::cerr << "You should check the partitions" << std::endl;
            return -1;
        }
        if (CacheFS_init(4, FBR, 0.5, 0.5))
        {
            std::cerr << "Error in legal CacheFS_init" << std::endl;
            return -1;
        }

        // Open the same file twice.
        int f1 = CacheFS_open(CHAPTER_2);
        int f2 = CacheFS_open(CHAPTER_2);
        if (f1 < 0 || f2 < 0)
        {
            std::cerr << "Error: You have a problem in opening legal files" << std::endl;
            return -1;
        }
        // Open file that not in /tmp.
        int f3 = CacheFS_open(NOT_TMP);
        if (f3 >= 0)
        {
            std::cerr << "You should not support files that are not in /tmp" << std::endl;
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

        // Read from the first file the first + second block.
        if (CacheFS_pread(f1, buf + 150, 4000, 150) != 4000)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }
        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        // Read from the illegal file.
        if (CacheFS_pread(f3, buf + 4150, 5000, 0) != -1)
        {
            std::cerr << "Error in CacheFS_pread while illegally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        if (CacheFS_pread(f2, buf + 4150, 5000, 0) != 5000)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        // Read from the first file chunk that is bigger then the file itself.
        if (CacheFS_pread(f1, buf + 9150, 7000, 15000) != 4090)
        {
            std::cerr << "Error in CacheFS_pread while reading with larger request" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        if (CacheFS_pread(f2, buf + 500, 150, 9000) != 150)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        if (CacheFS_pread(f2, buf, 3000, 12000) != 3000)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        if (CacheFS_pread(f1, buf, 50, 18000) != 50)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);


        if (CacheFS_pread(f1, buf, 4096, 0) != 4096)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);


        if (CacheFS_pread(f2, buf, 50000, 8192) != 10898)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        if (CacheFS_pread(f1, buf + 1000, 13, 4500) != 13)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);
        if (CacheFS_pread(f1, buf + 1000, 13, 4500) != 13)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }


        if (CacheFS_close(f1) != 0)
        {
            std::cerr << "Error in CacheFS_close while closing legal file" << std::endl;
            return -1;
        }
        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);
        if (CacheFS_pread(f1, buf + 1000, 13, 4500) != -1)
        {
            std::cerr << "Error in CacheFS_pread while illegally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        if (CacheFS_pread(f2, buf + 1000, 13, 4500) != 13)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }

        // Print Cache State.
        CacheFS_print_cache(CACHE_LOG_PATH);
        // Print Cache Stats.
        CacheFS_print_stat(STAT_LOG_PATH);

        if (CacheFS_pread(f2, buf, 20000, 0) != 19090)
        {
            std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
            return -1;
        }

        // Close the second file.
        if (CacheFS_close(f2) != 0)
        {
            std::cerr << "Error in CacheFS_close while closing legal file" << std::endl;
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




