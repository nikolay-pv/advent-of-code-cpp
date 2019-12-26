#pragma once
#include <utility>
#include <vector>
#include <set>

#include "intcode.hpp"

enum Direction : long
{
    Invalid = 0,
    North = 1,
    South = 2,
    West  = 3,
    East  = 4
};

enum OutputDroid : long
{
    Wall = 0,
    Normal = 1,
    Oxygen = 2
};

char getGlyph(OutputDroid od);

using coord = std::pair<long, long>;

struct Cell
{
    Cell(coord const& p, char m);
    coord pos{};
    char mark{' '};
    Direction returnToInitial{Invalid};
    std::list<Direction> unVisitedDirs{
                Direction::North, Direction::South,
                Direction::West, Direction::East};
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

struct RepairDroid
{
    IntCodeComputer& brain;
public:
    RepairDroid() = delete;
    RepairDroid(RepairDroid&&) = default;
    RepairDroid(const RepairDroid&) = default;
    ~RepairDroid() = default;

    RepairDroid(IntCodeComputer&);

public:

    //PaintColor getCurrentPlateColor() const;
    // returns true if the first time painting
    //bool paintCurrentPlate(PaintColor withColor);
    //void updatePosition(Rotation);
    void searchOxygen();
    void printMap(bool getBack) const;

public:
    bool emergencyHalt = false;
    set<CellPtr, CellPtrCmp> theMap{};
    CellPtr current{};
    CellPtr origin{};
    CellPtr oxygen{};
    Direction currentDirection{};
};

