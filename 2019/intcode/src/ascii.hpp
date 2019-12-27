#pragma once
#include <utility>
#include <vector>
#include <set>

#include "intcode.hpp"

namespace ascii {

const char newLine{10};

using coord = std::pair<long, long>;

struct Cell
{
    Cell(coord const& p, char m);
    coord pos{};
    char mark{' '};
    std::set<shared_ptr<Cell>> connectivity{};
    friend bool operator >(const Cell& l, const Cell& r)
    { return l.pos.first > r.pos.first && l.pos.second && l.pos.second; }
    friend bool operator <(const Cell& l, const Cell& r)
    { return !(l.pos > r.pos); }
    friend bool operator==(const Cell& l, const Cell& r)
    { return l.pos == r.pos; }
    friend bool operator!=(const Cell& l, const Cell& r)
    { return !(l == r); }
};
using CellPtr = shared_ptr<Cell>;

std::ostream& operator<<(std::ostream& os, const Cell& obj);


struct CellPtrCmp {
    bool operator()(const CellPtr& lhs, const CellPtr& rhs) const {
        return lhs->pos < rhs->pos;
    }
};

struct Ascii
{
    IntCodeComputer& brain;
public:
    Ascii() = delete;
    Ascii(Ascii&&) = default;
    Ascii(const Ascii&) = default;
    ~Ascii() = default;

    Ascii(IntCodeComputer&);

public:
    void constructMap();
    void findIntersections();
    void printMap(bool getBack) const;

public:
    vector<CellPtr> theMap{};
    set<CellPtr, CellPtrCmp> scafold{};
    set<CellPtr, CellPtrCmp> intersections{};
    coord position{0, 0};
    long width{100000};
};
} // namespace ascii
