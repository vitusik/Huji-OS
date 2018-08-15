#ifndef OS_EX3_WORDFREQUENCIESCLIENT_HPP
#define OS_EX3_WORDFREQUENCIESCLIENT_HPP

#include <fstream>
#include <iostream>
#include <sstream>

#include "../Framework/MapReduceClient.h"
#include "../Framework/MapReduceFramework.h"

class Line : public k1Base
{
private:
    const std::string line;
public:
    Line(const std::string& line) : line(line)
    {}

    virtual bool operator<(const k1Base& other) const
    {
        return this->line < ((Line&) other).line;
    }

    const std::string& getLine()
    {
        return line;
    }
};

class Word : public k2Base, public k3Base
{
private:
    const std::string word;
public:
    Word(const std::string& word) : word(word)
    {}

    Word(const Word& other) = default;

    virtual bool operator<(const k2Base& other) const
    {
        return this->word < ((Word&) other).word;
    }

    virtual bool operator<(const k3Base& other) const
    {
        return this->word < ((Word&) other).word;
    }

    const std::string& getWord()
    {
        return word;
    }

};

class Integer : public v3Base
{
public:
    int val;

    Integer(int val) : val(val)
    {}
};

class MapReduceWordFrequencies : public MapReduceBase
{
    virtual void Map(const k1Base* const key, const v1Base* const val) const
    {
        std::stringstream sstream(((Line*) key)->getLine());
        std::string word;
        while (sstream >> word)
        {
            Word* k2 = new Word(word);
            Emit2(k2, nullptr);
        }
    }

    virtual void Reduce(const k2Base* const key, const V2_VEC& vals) const
    {
        Word* k3 = new Word(((Word&) *key));
        Integer* frequency = new Integer(vals.size());
        Emit3(k3, frequency);
    }
};

#endif //OS_EX3_WORDFREQUENCIESCLIENT_HPP
