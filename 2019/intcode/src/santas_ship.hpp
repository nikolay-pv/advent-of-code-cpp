#pragma once
#include <memory>
#include <set>
#include <utility>
#include <vector>
#include <list>
#include <string>
#include <queue>

class IntCodeComputer;

namespace santa {
using coord = std::pair<long, long>;
struct Cell;
using CellPtr = std::shared_ptr<Cell>;

struct Cell
{
    Cell(coord const& p, char m);
    Cell(coord const& p, char m, char orig);

    coord pos{};
    char mark{' '};
    char originalMark{'.'};
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


struct SantasShip
{
public:
    SantasShip(IntCodeComputer&);

    void fly();
    void printMap() const;
    void printAll() const;

private:
    void loadHistoryCommands();
    void addNewCell(coord offset);
    CellPtr findCell(coord offset) const;
    void updateCurrent(CellPtr n);
    void processOutput();
    void processInput();

private:
    IntCodeComputer& mapProvider;

    CellPtr currentPos;
    CellPtr previousPos;
    long pos{};
    mutable long linesBack{};
    std::string previousInput{};
    std::string input{};
    std::string output{};
    std::queue<std::string> commands{};
    std::set<CellPtr, CellPtrCmp> theMap{};
};

} // namespace santa
