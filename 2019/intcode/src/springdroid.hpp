#pragma once
#include <memory>
#include <set>
#include <utility>
#include <vector>
#include <list>

class IntCodeComputer;

namespace spring {
using coord = std::pair<long, long>;
struct Cell;
using CellPtr = std::shared_ptr<Cell>;

struct Cell
{
    Cell(coord const& p, char m);

    coord pos{};
    char mark{' '};
    std::set<std::shared_ptr<Cell>> connectivity{};

    friend bool operator >(const Cell& l, const Cell& r)
    { return l.pos.first > r.pos.first && l.pos.second && l.pos.second; }
    friend bool operator <(const Cell& l, const Cell& r)
    { return !(l.pos > r.pos); }
    friend bool operator==(const Cell& l, const Cell& r)
    { return l.pos == r.pos; }
    friend bool operator!=(const Cell& l, const Cell& r)
    { return !(l == r); }
};

std::ostream& operator<<(std::ostream& os, const Cell& obj);

struct CellPtrCmp {
    bool operator()(const CellPtr& lhs, const CellPtr& rhs) const {
        return lhs->pos < rhs->pos;
    }
};


struct SpringDroid 
{
    IntCodeComputer& brain;
    SpringDroid(IntCodeComputer&);

public:
    void run();
    void printMap(bool getBack = false) const;
public:
    long damage{};
    std::list<long> previous;
    std::vector<CellPtr> theMap{};
};

}// namespace spring
