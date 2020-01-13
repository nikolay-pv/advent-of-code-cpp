#include <iostream>
#include <iomanip>
#include <vector>
#include <array>
//#include <valarray>
//#include <map>
#include <set>
#include <list>
//#include <list>
//#include <memory>
#include <cmath>
#include <numeric>
//#include <sstream>
#include <optional>
#include <chrono>
#include <thread>
#include <bitset>
//#include <queue>

using namespace std;

constexpr long mapSize = 25;
constexpr long mid = 12;

const bitset<mapSize> leftEdge{"1000010000100001000010000"};
const bitset<mapSize> topEdge{"1111100000000000000000000"};
const bitset<mapSize> rightEdge{"0000100001000010000100001"};
const bitset<mapSize> bottomEdge{"0000000000000000000011111"};
const bitset<mapSize> allEdges{leftEdge | topEdge | rightEdge | bottomEdge};

const bitset<mapSize> leftInternalEdge{"0000000000010000000000000"};
const bitset<mapSize> topInternalEdge{"0000000100000000000000000"};
const bitset<mapSize> rightInternalEdge{"0000000000000100000000000"};
const bitset<mapSize> bottomInternalEdge{"0000000000000000010000000"};
const bitset<mapSize> allInternalEdges{leftInternalEdge | topInternalEdge | rightInternalEdge | bottomInternalEdge};

struct BitCmp{
    bool operator ()(const bitset<mapSize>& l, const bitset<mapSize>& r) const
    { return l.to_ulong() < r.to_ulong(); }
};
set<bitset<mapSize>, BitCmp> history{};


struct Eris;
using ErisPtr = shared_ptr<Eris>;
list<ErisPtr> planet_system{};

struct Eris
{
    Eris() = default;
    Eris(const string& input) : theMap{input.c_str(), mapSize, '.', '#'} { history.insert(theMap); }

    void printMap(bool goBack = false) const;

    long countNeighbours(long i) const;
    long countEmbeddedNeighbours(long i) const;
    bitset<mapSize> getNext() const;
    void runSimulation();
    unsigned long biodiversityRating() const;

    // part 2
    void calculateEmbeddedNext(optional<bool> goingdown = nullopt);
    long long countBugs(optional<bool> goingdown = nullopt) const;
    // recursive update
    long long runEmbeddedSimulation(long times);

    Eris* down{};
    Eris* up{};

    bitset<mapSize> theMap;
    long time{};
};

std::ostream& operator<<(std::ostream& os, const Eris& obj)
{
    static const array<size_t, 4> poss{5,11,17,23};
    string tmp{obj.theMap.to_string('.', '#')};
    for(size_t p : poss)
        tmp.insert(p, 1, '\n');
    os << tmp;
    return os;
}

void Eris::printMap(bool goBack) const
{
    cout << "Time " << time << " m" << endl;
    cout << *this;
    if (goBack)
        cout << "\033[" << 5 << "A";
}

long Eris::countNeighbours(long i) const
{
    long bugs{};
    const array<long, 4> positions{i - 1, i + 1, i - 5, i + 5};
    for(auto idx : positions)
    {
        const bool horizontal = abs(idx - i) == 1 ? true : false;
        if (idx < 0 || idx >= mapSize
            || (horizontal && idx / 5 != i / 5)) // +- 1
            continue;
        if (theMap[idx]) // bug
            ++bugs;
    }
    return bugs;
}

unsigned long Eris::biodiversityRating() const
{
    bitset<mapSize> tmp{};
    for(int i = 0; i != mapSize; ++i)
        tmp[i] = theMap[mapSize - i - 1];
    return tmp.to_ulong();
}

bitset<mapSize> Eris::getNext() const
{
    bitset<mapSize> newMap{};
    for(long i = 0; i != mapSize; ++i)
    {
        const long bugs = countNeighbours(i);
        if (theMap[i] && bugs != 1) // bug
            newMap[i] = 0;
        else if (!theMap[i] && (bugs == 1 || bugs == 2)) // empty
            newMap[i] = 1;
        else
            newMap[i] = theMap[i];
    }
    return newMap;
}


// count embedded if at the edge
long Eris::countEmbeddedNeighbours(long i) const
{
    bitset<mapSize> test{};
    test[i] = 1;
    long res{};
    if (down && (allInternalEdges & test).any())
    {
        // down the level
        if ((test & topInternalEdge).any())
            res = (down->theMap & topEdge).count();
        else if ((test & bottomInternalEdge).any())
            res = (down->theMap & bottomEdge).count();
        else if ((test & leftInternalEdge).any())
            res = (down->theMap & leftEdge).count();
        else if ((test & rightInternalEdge).any())
            res = (down->theMap & rightEdge).count();
    }
    else if (up && (allEdges & test).any())
    {
        // up the level
        if ((test & topEdge).any())
            res += (up->theMap & topInternalEdge).count();
        if ((test & bottomEdge).any())
            res += (up->theMap & bottomInternalEdge).count();
        if ((test & leftEdge).any())
            res += (up->theMap & leftInternalEdge).count();
        if ((test & rightEdge).any())
            res += (up->theMap & rightInternalEdge).count();
    }
    return res;
}

void Eris::calculateEmbeddedNext(optional<bool> goingdown)
{
    bitset<mapSize> newMap{};
    for(long i = 0; i != mapSize; ++i)
    {
        if (i == mid)
            continue;
        const long bugs = countNeighbours(i) + countEmbeddedNeighbours(i);
        if (theMap[i] && bugs != 1) // bug
            newMap[i] = 0;
        else if (!theMap[i] && (bugs == 1 || bugs == 2)) // empty
            newMap[i] = 1;
        else
            newMap[i] = theMap[i];
    }
    // recursive call
    const bool godown = goingdown == nullopt || goingdown.value();
    if (godown && (down || (theMap & allInternalEdges).count()))
    {
        if (!down)
        {
            auto n = make_shared<Eris>();
            n->up = this;
            down = n.get();
            planet_system.push_back(n);
        }
        down->calculateEmbeddedNext(true);
    }
    //
    const bool goup = goingdown == nullopt || !goingdown.value();
    if (goup && (up || (theMap & allEdges).count()))
    {
        if (!up)
        {
            auto n = make_shared<Eris>();
            n->down = this;
            up = n.get();
            planet_system.push_back(n);
        }
        up->calculateEmbeddedNext(false);
    }
    theMap = newMap;
}

void Eris::runSimulation()
{
    while(true)
    {
        auto newMap = getNext();
        auto f = history.insert(newMap);
        theMap = newMap;
        ++time;
        if (!f.second)
        {
            printMap(false);
            unsigned long answ = biodiversityRating();
            cout << endl << "Duplicate found with biodiversity rating equals to " << answ << endl;
            assert(answ > 11184457);
            return;
        }
        printMap(true);
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

long long Eris::countBugs(optional<bool> goingdown) const
{
    long long answ = theMap.count();

    const bool godown = goingdown == nullopt || goingdown.value();
    if (godown && down)
        answ += down->countBugs(true);

    const bool goup = goingdown == nullopt || !goingdown.value();
    if (goup && up)
        answ += up->countBugs(false);

    return answ;
}

long long Eris::runEmbeddedSimulation(long times)
{
    for(long count{}; count != times; ++count)
        calculateEmbeddedNext();
    auto answ{countBugs()};

    cout << "Total amount of bugs after " << times << " iterations is " << answ << endl;
    return answ;
}

////////////////////////////////////////////////////////////////////////////////
// main
int main()
{
    //int N;
    //cin >> N;
    string input;
    std::getline(cin, input);

    //{
    //    cout << "Test\n";
    //    Eris test{"....##..#.#..##..#..#...."};
    //    test.printMap();
    //    array<long, 25> bugs{0,0,1,1,0,
    //                         1,2,0,1,2,
    //                         1,2,2,1,1,
    //                         3,1,1,1,1,
    //                         0,2,0,0,1};
    //    auto newMap = test.getNext();
    //    bitset<mapSize> first{"#..#.####.###.###.##.##..", mapSize, '.', '#'};
    //    for(int i = 0; i != mapSize; ++i)
    //    {
    //        const long b = test.countNeighbours(i);
    //        //cerr << "compare for " << i << " got " << b << " expected " << bugs[i] << endl;
    //        assert(b == bugs[i]);
    //        // test iteration
    //        assert(first[i] == newMap[i]);
    //    }
    //}

    //{
    //    cout << "Test part 2\n";
    //    Eris test{"....##..#.#..##..#..#...."};
    //    const long times = 10;
    //    const auto answ = test.runEmbeddedSimulation(times);
    //    test.printMap();
    //    test.down->printMap();
    //    assert(answ == 99);
    //}

    cout << "Part 1\n";
    {
    Eris planet{input};
    planet.printMap();
    cout << "\nRun sim " << endl;
    planet.runSimulation();
    }
    cout << "Part 2\n";
    {
    const long times = 200;
    Eris planet{input};
    const long answ = planet.runEmbeddedSimulation(times);
    assert(1450 < answ);
    }
    return 0;
}

