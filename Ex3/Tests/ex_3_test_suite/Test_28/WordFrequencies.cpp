#include "WordFrequenciesClient.hpp"
#include <algorithm>

#define NUM_THREADS 8

void writeByFrequency(OUT_ITEMS_VEC& frequencies, std::ofstream& ofs)
{
    // get length of longest word (so we can write to the file in a nice format)
    unsigned int maxLength = 0;
    for (auto it = frequencies.begin(); it != frequencies.end(); ++it)
    {
        unsigned int length = (*(Word*) it->first).getWord().length();
        maxLength = length > maxLength ? length : maxLength;
    }

    // sort by frequency is descending order
    std::sort(
            frequencies.begin(),
            frequencies.end(),
            [](const OUT_ITEM& o1, const OUT_ITEM& o2)
            {
                // o1 comes before o2 if the frequency of o1 is higher OR
                // they have the same frequency and o1 comes before o2 in
                // lexicographic order

                if (((Integer*) o1.second)->val < ((Integer*) o2.second)->val)
                    return false;

                return ((Integer*) o1.second)->val > ((Integer*) o2.second)->val
                        || ((Word*)o1.first)->getWord() < ((Word*)o2.first)->getWord();
            }
    );

    // writing results to file
    for (auto it = frequencies.begin(); it != frequencies.end(); ++it)
    {
        const std::string& word = (*(Word*) it->first).getWord();
        int frequency = ((Integer*) it->second)->val;
        ofs << word;
        for (int i = word.length(); i < maxLength + 5; ++i)
        {
            // pad with spaces
            ofs << " ";
        }
        ofs << frequency << std::endl;
    }
}

void findWordFrequencies(std::ifstream& ifs, std::ofstream& ofs)
{
    IN_ITEMS_VEC k1v1Pairs;

    // make the input for the framework
    std::string line;
    while (std::getline(ifs, line))
    {
        Line* k1 = new Line(line);
        k1v1Pairs.push_back(IN_ITEM(k1, nullptr));
    }

    MapReduceWordFrequencies mapReduceObj;

    OUT_ITEMS_VEC frequencies = RunMapReduceFramework
            (mapReduceObj, k1v1Pairs, NUM_THREADS, true);


    writeByFrequency(frequencies, ofs);

}


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: WordFrequencies <path/to/text_file>" << std::endl;
        return 1;
    }

    std::string textPath(argv[1]);

    std::ifstream ifs(textPath);
    std::ofstream ofs(textPath + std::string("_test_results"),
                      std::ofstream::out);

    if (!ifs.is_open() || !ofs.is_open())
    {
        std::cerr << "I/O Error occurred - couldn't open input or output file"
                  << std::endl;
        return 1;
    }


    findWordFrequencies(ifs, ofs);
    ifs.close();
    ofs.close();

}