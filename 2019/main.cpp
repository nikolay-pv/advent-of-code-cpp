#include <iostream>
#include <iomanip>
#include <vector>
#include <array>
//#include <valarray>
//#include <map>
#include <set>
//#include <list>
//#include <list>
//#include <memory>
#include <cmath>
#include <numeric>
//#include <sstream>
//#include <optional>
#include <chrono>
#include <thread>
#include <bitset>
//#include <queue>

using namespace std;

constexpr long mapSize = 25;

struct BitCmp{
    bool operator ()(const bitset<mapSize>& l, const bitset<mapSize>& r) const
    { return l.to_ulong() < r.to_ulong(); }
};

struct Eris
{
    Eris(const string& input) : theMap{input.c_str(), mapSize, '.', '#'} { history.insert(theMap); }

    void printMap(bool goBack = false) const;

    pair<long, long> countNeighbours(long i) const;
    bitset<mapSize> getNext() const;
    void runSimulation();

    unsigned long biodiversityRating() const;

    bitset<mapSize> theMap;
    long time{};
    set<bitset<mapSize>, BitCmp> history{};
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

pair<long, long> Eris::countNeighbours(long i) const
{
    assert(!theMap.none());
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
    return {bugs, 4 - bugs};
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
        auto[bugs, empty] = countNeighbours(i);
        if (theMap[i] && bugs != 1) // bug
            newMap[i] = 0;
        else if (!theMap[i] && (bugs == 1 || bugs == 2)) // empty
            newMap[i] = 1;
        else
            newMap[i] = theMap[i];
    }
    return newMap;
}

void Eris::runSimulation()
{
    //long count = 4;
    //while(count--)
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
    //        auto[b,e] = test.countNeighbours(i);
    //        //cerr << "compare for " << i << " got " << b << " expected " << bugs[i] << endl;
    //        assert(b == bugs[i]);
    //        // test iteration
    //        assert(first[i] == newMap[i]);
    //    }
    //}

    cout << "Part 1\n";
    Eris planet{input};
    planet.printMap();
    cout << "\nRun sim " << endl;
    planet.runSimulation();
    //cout << "Part 2\n";

    return 0;
}

