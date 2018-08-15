#ifndef TESTS_TEST6MAPREDUCE_HPP
#define TESTS_TEST6MAPREDUCE_HPP

#include <iostream>
#include <sstream>
#include <list>

#include "GridKeysValues.hpp"


class GridMapReduce : public MapReduceBase
{
public:

    GridMapReduce() = default;


    /**
     * @param key row number
     * @param val the row itself (string)
     */
    virtual void Map(const k1Base* const key, const v1Base* const val)
    const override
    {
        int v, max;
        std::stringstream sstream(((RowString*) val)->getRow());

        // find the maximum value of this row
        sstream >> max;
        while (sstream >> v)
        {
            if (v > max)
            {
                max = v;
            }
        }


        // reset stream
        sstream = std::stringstream(((RowString*) val)->getRow());

        // find all points in this line with the maximum value
        // (all Row-King-Midgets)
        int col = 1;
        while (sstream >> v)
        {
            if (v == max)
            {
                // memory should be freed by the framework
                Emit2(new Index(col),
                      new RowMaxVal(((Index*) key)->getIndex(), max));
            }

            col++;
        }
    }

    /**
     *
     * @param key column number j
     * @param vals container of pairs - each pair is a row number i and the
     *        height of the point (i,j)
     *        (these are potential candidates to be Shaman-Midgets)
     */
    virtual void Reduce(const k2Base* const key, const V2_VEC& vals)
    const override
    {
        int v, max;
        int col = ((Index*) key)->getIndex();

        // initialize max with the value of the first point
        max = ((const RowMaxVal*) vals.front())->getValue();

        //find the maximum value in this column
        for (auto it = vals.begin(); it != vals.end(); it++)
        {
            v = ((RowMaxVal*) (*it))->getValue();
            if (v > max)
            {
                max = v;
            }
        }


        for (auto it = vals.begin(); it != vals.end(); it++)
        {
            v = ((RowMaxVal*) (*it))->getValue();

            if (v == max)
            {
                // this memory should be freed by whoever called the
                // framework and holds the vector of results
                GridPoint* point =
                        new GridPoint(((RowMaxVal*) (*it))->getRowNum(), col);
                GridPointVal* pointVal = new GridPointVal(v);
                Emit3(point, pointVal);
            }
        }

    }
};


#endif //TESTS_TEST6MAPREDUCE_HPP
