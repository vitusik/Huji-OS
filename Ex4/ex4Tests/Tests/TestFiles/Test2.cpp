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
#define CACHE_LOG_PATH "CacheLog2"
#define STAT_LOG_PATH "StatLog2"
#define BUFFER_OUTPUT "BufferOutput2"


int main(int argc, char *argv[])
{
    for (int i = 0; i < 2; ++i)
    {
        try
        {
            char *buf = nullptr;
            buf = new char[BUFFER_SIZE];
            std::ofstream bufferOutput(BUFFER_OUTPUT);

            // Start LRU Library.
            if (CacheFS_init(5, LFU, 0, 3))
            {
                std::cerr << "Error: You should not care about f_old/f_new in LFU" << std::endl;
                return -1;
            }

            // Open the same file twice.
            int f1 = CacheFS_open(CHAPTER_1);
            int f2 = CacheFS_open(CHAPTER_2);
            int f3 = CacheFS_open(CHAPTER_2);
            if (f1 < 0 || f2 < 0 || f3 < 0)
            {
                std::cerr << "Error: You have a problem in opening legal files" << std::endl;
                return -1;
            }
            // Open file that not in /tmp.
            int f4 = CacheFS_open(NOT_TMP);

            if (f4 >= 0)
            {
                std::cerr << "Error: You should not support files that are not in /tmp" << std::endl;
                return -1;
            }

            // Print Cache State.
            CacheFS_print_cache(CACHE_LOG_PATH);
            // Print Cache Stats.
            CacheFS_print_stat(STAT_LOG_PATH);

            // Read from the first file the first block.
            if (CacheFS_pread(f1, buf, 150, 0) != 150)
            {
                std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
                return -1;
            }

            // Print Cache State.
            CacheFS_print_cache(CACHE_LOG_PATH);
            // Print Cache Stats.
            CacheFS_print_stat(STAT_LOG_PATH);


            if (CacheFS_pread(f2, buf + 150, 5000, 0) != 5000)
            {
                std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
                return -1;
            }

            // Print Cache State.
            CacheFS_print_cache(CACHE_LOG_PATH);
            // Print Cache Stats.
            CacheFS_print_stat(STAT_LOG_PATH);

            // Read from illegal file.
            if (CacheFS_pread(f4, buf + 5150, 5000, 0) != -1)
            {
                std::cerr << "Error in CacheFS_pread while illegally reading" << std::endl;
                return -1;
            }

            // Print Cache State.
            CacheFS_print_cache(CACHE_LOG_PATH);
            // Print Cache Stats.
            CacheFS_print_stat(STAT_LOG_PATH);

            if (CacheFS_pread(f3, buf + 5150, 5000, 4096) != 5000)
            {
                std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
                return -1;
            }
            // Print Cache State.
            CacheFS_print_cache(CACHE_LOG_PATH);
            // Print Cache Stats.
            CacheFS_print_stat(STAT_LOG_PATH);

            if (CacheFS_pread(f1, buf + 10150, 7000, 20000) != 5832)
            {
                std::cerr << "Error in CacheFS_pread while reading with request that is larger then the file size" << std::endl;
                return -1;
            }

            // Print Cache State.
            CacheFS_print_cache(CACHE_LOG_PATH);
            // Print Cache Stats.
            CacheFS_print_stat(STAT_LOG_PATH);

            if (CacheFS_pread(f2, buf + 15982, 18, 12000) != 18)
            {
                std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
                return -1;
            }

            // Print Cache State.
            CacheFS_print_cache(CACHE_LOG_PATH);
            // Print Cache Stats.
            CacheFS_print_stat(STAT_LOG_PATH);

            if (CacheFS_pread(f3, buf + 15982, 18, 12000) != 18)
            {
                std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
                return -1;
            }

            // Print Cache State.
            CacheFS_print_cache(CACHE_LOG_PATH);
            // Print Cache Stats.
            CacheFS_print_stat(STAT_LOG_PATH);

            if (CacheFS_pread(f3, buf + 16000, 4500, 16000) != 3090)
            {
                std::cerr << "Error in CacheFS_pread while reading with request that is larger then the file size" << std::endl;
                return -1;
            }

            // Print Cache State.
            CacheFS_print_cache(CACHE_LOG_PATH);
            // Print Cache Stats.
            CacheFS_print_stat(STAT_LOG_PATH);

            if (CacheFS_pread(f1, buf + 19090, 110, 16000) != 110)
            {
                std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
                return -1;
            }

            // Print Cache State.
            CacheFS_print_cache(CACHE_LOG_PATH);
            // Print Cache Stats.
            CacheFS_print_stat(STAT_LOG_PATH);

            if (CacheFS_pread(f1, buf + 19200, 1, 16000) != 1)
            {
                std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
                return -1;
            }

            // Print Cache State.
            CacheFS_print_cache(CACHE_LOG_PATH);
            // Print Cache Stats.
            CacheFS_print_stat(STAT_LOG_PATH);

            if (CacheFS_pread(f1, buf + 19201, 4, 16000) != 4)
            {
                std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
                return -1;
            }

            // Print Cache State.
            CacheFS_print_cache(CACHE_LOG_PATH);
            // Print Cache Stats.
            CacheFS_print_stat(STAT_LOG_PATH);

            if (CacheFS_pread(f1, buf + 19205, 0, 16000) != 0)
            {
                std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
                return -1;
            }

            // Print Cache State.
            CacheFS_print_cache(CACHE_LOG_PATH);
            // Print Cache Stats.
            CacheFS_print_stat(STAT_LOG_PATH);

            // Read from the first file the first block.
            if (CacheFS_pread(f1, buf + 19205, 150, 0) != 150)
            {
                std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
                return -1;
            }

            // Print Cache State.
            CacheFS_print_cache(CACHE_LOG_PATH);
            // Print Cache Stats.
            CacheFS_print_stat(STAT_LOG_PATH);

            // Read from the first file the first block.
            if (CacheFS_pread(f2, buf + 19355, 155, 0) != 155)
            {
                std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
                return -1;
            }

            // Print Cache State.
            CacheFS_print_cache(CACHE_LOG_PATH);
            // Print Cache Stats.
            CacheFS_print_stat(STAT_LOG_PATH);

            // Read from the first file the first block.
            if (CacheFS_pread(f3, buf + 19500, 10, 0) != 10)
            {
                std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
                return -1;
            }

            // Print Cache State.
            CacheFS_print_cache(CACHE_LOG_PATH);
            // Print Cache Stats.
            CacheFS_print_stat(STAT_LOG_PATH);

            // Read from the first file the first block.
            if (CacheFS_pread(f3, buf + 19500, 10, 0) != 10)
            {
                std::cerr << "Error in CacheFS_pread while legally reading" << std::endl;
                return -1;
            }

            if (CacheFS_close(f1) != 0)
            {
                std::cerr << "Error in CacheFS_close while closing legal file" << std::endl;
                return -1;
            }

            // Close the second file.
            if (CacheFS_close(f2) != 0)
            {
                std::cerr << "Error in CacheFS_close while closing legal file" << std::endl;
                return -1;
            }

            if (CacheFS_close(f3) != 0)
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
            // Close an already closed file.
            if (CacheFS_close(15) != -1)
            {
                std::cerr << "Error in CacheFS_close while closing illegal file" << std::endl;
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

        }
        catch (std::bad_alloc &exception)
        {
            std::cerr << "Exception: " << exception.what() << std::endl;
        }
    }

    return 0;

}




