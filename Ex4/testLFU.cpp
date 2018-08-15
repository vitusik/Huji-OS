
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <cstring>
#include "CacheFS.h"

void basicLFU()
{
    bool ok = true;

    // get the block size:
    struct stat fi;
    stat("/tmp", &fi);
    size_t blockSize = (size_t)fi.st_blksize;

    // create the files for the test:
    std::ofstream outfile1 ("/tmp/LFU1.txt");
    for (unsigned int i=0; i<5*blockSize; i++)
    {
        outfile1 << "A";
    }
    outfile1.close();
    std::ofstream outfile2 ("/tmp/LFU2.txt");
    for (unsigned int i=0; i<5*blockSize; i++)
    {
        outfile2 << "B";
    }
    outfile2.close();

    std::ofstream eraser;
    eraser.open("/tmp/LFU_cache.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();
    eraser.open("/tmp/LFU_stats.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();

    CacheFS_init(5, LFU, 0.1, 0.1);
    int fd1 = CacheFS_open("/tmp/LFU1.txt");
    int fd2 = CacheFS_open("/tmp/LFU2.txt");

    char data[11];

    // ramp up the frequency of these ones:
    CacheFS_pread(fd1, &data, 10, 0*blockSize);
    CacheFS_pread(fd1, &data, 10, 1*blockSize);
    CacheFS_pread(fd1, &data, 10, 2*blockSize);
    CacheFS_pread(fd1, &data, 10, 3*blockSize);
    CacheFS_pread(fd1, &data, 10, 4*blockSize);
    CacheFS_pread(fd1, &data, 10, 0*blockSize);
    CacheFS_pread(fd1, &data, 10, 0*blockSize);
    CacheFS_pread(fd1, &data, 10, 0*blockSize);
    CacheFS_pread(fd1, &data, 10, 0*blockSize);
    CacheFS_pread(fd1, &data, 10, 2*blockSize);
    CacheFS_pread(fd1, &data, 10, 2*blockSize);
    CacheFS_pread(fd1, &data, 10, 2*blockSize);
    CacheFS_pread(fd1, &data, 10, 4*blockSize);
    CacheFS_pread(fd1, &data, 10, 4*blockSize);
    CacheFS_pread(fd1, &data, 10, 1*blockSize);
    CacheFS_print_cache("/tmp/LFU_cache.txt");

    // these should all be misses:
    CacheFS_pread(fd2, &data, 10, 0*blockSize);
    CacheFS_print_cache("/tmp/LFU_cache.txt");
    CacheFS_pread(fd2, &data, 10, 1*blockSize);
    CacheFS_print_cache("/tmp/LFU_cache.txt");
    CacheFS_pread(fd2, &data, 10, 0*blockSize);
    CacheFS_pread(fd2, &data, 10, 1*blockSize);
    CacheFS_pread(fd2, &data, 10, 0*blockSize);
    CacheFS_pread(fd2, &data, 10, 1*blockSize);
    CacheFS_pread(fd2, &data, 10, 0*blockSize);
    CacheFS_pread(fd2, &data, 10, 1*blockSize);
    CacheFS_pread(fd2, &data, 10, 0*blockSize);
    CacheFS_pread(fd2, &data, 10, 1*blockSize);

    // get the results:
    CacheFS_print_stat("/tmp/LFU_stats.txt");

    // review cache:
    std::ifstream resultsFileInput;
    resultsFileInput.open("/tmp/LFU_cache.txt");
    char cacheResults[10000] = "\0";
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(cacheResults, 10000);

        char cacheCorrect[] = "/tmp/LFU1.txt 0\n/tmp/LFU1.txt "
                "2\n/tmp/LFU1.txt 4\n/tmp/LFU1.txt 1\n/tmp/LFU1.txt"
                " 3\n"
                "/tmp/LFU1.txt 0\n/tmp/LFU1.txt "
                "2\n/tmp/LFU1.txt 4\n/tmp/LFU1.txt 1\n/tmp/LFU2.txt"
                " 0\n"
                "/tmp/LFU1.txt 0\n/tmp/LFU1.txt "
                "2\n/tmp/LFU1.txt 4\n/tmp/LFU1.txt 1\n/tmp/LFU2.txt"
                " 1\n";

        if (strcmp(cacheResults, cacheCorrect)) {ok = false;}
    }
    resultsFileInput.close();

    // review stats:
    resultsFileInput.open("/tmp/LFU_stats.txt");
    char statsResults[10000] = "\0";
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(statsResults, 10000);

        if (!(!strcmp(statsResults, "Hits number: 10.\nMisses number: 15.\n") || !strcmp(statsResults, "Hits number: 10\nMisses number: 15\n")))
        {
            ok = false;
        }
    }
    resultsFileInput.close();

    CacheFS_close(fd1);
    CacheFS_close(fd2);
    CacheFS_destroy();

    if (ok)
    {
        std::cout << "Basic LFU Check Passed!\n";
    }
    else
    {
        std::cout << "Basic LFU Check Failed!\n";
    }
}


int main(){

    basicLFU();
    return 0;
}