#ifndef OS_EX3_TEST6KEYSVALUES_HPP
#define OS_EX3_TEST6KEYSVALUES_HPP


#include <string>
#include "../Framework/MapReduceClient.h"
#include "../Framework/MapReduceFramework.h"


class Index : public k1Base, public k2Base
{
private:
    int index;

public:
    Index(int index)
    {
        this->index = index;
    }

    virtual bool operator<(const k1Base& other) const override
    {
        return this->index < ((Index&) other).index;
    }

    virtual bool operator<(const k2Base& other) const override
    {
        return this->index < ((Index&) other).index;
    }

    int getIndex() const
    {
        return index;
    }

};

class GridPoint : public k3Base
{
private:
    int row, col;

public:
    GridPoint(int row, int col)
    {
        this->row = row;
        this->col = col;
    }

    virtual bool operator<(const k3Base& other) const override
    {
        if (this->row < ((GridPoint&) other).row)
            return true;

        return (this->row == ((GridPoint&) other).row) &&
               (this->col < ((GridPoint&) other).col);
    }

    int getRow() const
    {
        return row;
    }

    int getCol() const
    {
        return col;
    }
};


class RowString : public v1Base
{
private:
    std::string row;

public:
    RowString(const char* row)
    {
        this->row = row;
    }

    RowString(const std::string& row)
    {
        this->row = row;
    }

    const std::string& getRow() const
    {
        return row;
    }
};


/**
 * a maximum value of a particular row
 */
class RowMaxVal : public v2Base
{
private:
    int rowNum;
    int value;

public:
    RowMaxVal(int rowNum, int value)
    {
        this->rowNum = rowNum;
        this->value = value;
    }

    int getRowNum() const
    {
        return rowNum;
    }

    int getValue() const
    {
        return value;
    }

};


class GridPointVal : public v3Base
{
private:
    int value;

public:
    GridPointVal(int value)
    {
        this->value = value;
    }

    int getValue() const
    {
        return value;
    }

};


#endif //OS_EX3_TEST6KEYSVALUES_HPP
