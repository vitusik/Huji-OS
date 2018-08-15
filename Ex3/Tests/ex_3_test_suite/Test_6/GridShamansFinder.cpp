#include <fstream>
#include <iostream>
#include "GridMapReduce.hpp"


#define NUM_THREADS 8


void runSingleGrid(std::ifstream& ifs, std::ofstream& ofs)
{
    IN_ITEMS_VEC k1v1Pairs;

    // create the input vector
    int row = 1;
    std::string line;
    while (std::getline(ifs, line))
    {
        Index* k = new Index(row);
        RowString* v = new RowString(line);
        k1v1Pairs.push_back(IN_ITEM(k, v));

        row++;
    }


    // create MapReduceBase object
    GridMapReduce gmr;

    // let the framework do the magic
    OUT_ITEMS_VEC shamans =
            RunMapReduceFramework(gmr, k1v1Pairs, NUM_THREADS, true);


    // writing results to file
    for (auto it = shamans.begin(); it != shamans.end(); ++it)
    {
        GridPoint& point = *(GridPoint*) (it->first);
        int value = ((GridPointVal*) it->second)->getValue();
        ofs << point.getRow() << " " << point.getCol()
            << " " << value << std::endl;

    }

}


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: GridShamansFinder <path/to/grid>" << std::endl;
        return 1;
    }

    std::string gridPath(argv[1]);

    std::ifstream ifs(gridPath);
    std::ofstream ofs(gridPath + std::string("_test_results"),
                      std::ofstream::out);

    if (!ifs.is_open() || !ofs.is_open())
    {
        std::cerr << "I/O Error occurred - couldn't open input or output file"
                  << std::endl;
        return 0;
    }


    runSingleGrid(ifs, ofs);
    ifs.close();
    ofs.close();
}