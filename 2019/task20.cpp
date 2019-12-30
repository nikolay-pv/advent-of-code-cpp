#include <iostream>
#include <vector>
//#include <array>
//#include <valarray>
#include <map>
#include <set>
//#include <list>
//#include <memory>
#include <cmath>
#include <numeric>
//#include <sstream>
#include <optional>
#include <chrono>
#include <thread>
#include <queue>

using namespace std;
using coord = std::pair<long, long>;

bool operator >(const coord& l, const coord& r)
{ return l.first > r.first && l.second > r.second; }
bool operator <(const coord& l, const coord& r)
{ return !(l > r); }

constexpr long maxDepth = 65;

struct Cell;
using CellPtr = shared_ptr<Cell>;

struct Cell
{
    Cell(coord const& p, char m)
        : pos{p}, mark{m}
    { }

    coord pos{};
    char mark{' '};
    long levelChange{};
    string portal{};
    std::set<CellPtr> connectivity{};
    CellPtr portalCell{};

    friend bool operator >(const Cell& l, const Cell& r)
    { return l.pos.first > r.pos.first && l.pos.second && l.pos.second; }
    friend bool operator <(const Cell& l, const Cell& r)
    { return !(l.pos > r.pos); }
    friend bool operator==(const Cell& l, const Cell& r)
    { return l.pos == r.pos; }
    friend bool operator!=(const Cell& l, const Cell& r)
    { return !(l == r); }
};

std::ostream& operator<<(std::ostream& os, const Cell& obj)
{
    os << "Cell: " << obj.pos.first << " " << obj.pos.second << " with mark " << obj.mark;
    return os;
}

struct CellData
{
    CellData(CellPtr const& c, long l, long d)
        : cell{c}, level{l}, distance{d}
    {}

    CellPtr cell;
    //CellPtr comingFrom;
    long level;
    long distance;

    friend bool operator <(const CellData& l, const CellData& r)
    {
        return l.cell == r.cell
            ? l.level < r.level
            : (l.cell->pos.first <= r.cell->pos.first || l.cell->pos.second <= r.cell->pos.second);
    }
};

struct CellDataCmp {
    bool operator()(const CellData& lhs, const CellData& rhs) const {
        return lhs.level < rhs.level || lhs.distance < rhs.distance;
        //return lhs.cell == rhs.cell ? lhs.level < rhs.level : lhs.distance < rhs.distance;
    }
};

struct CellPtrCmp {
    bool operator()(const CellPtr& lhs, const CellPtr& rhs) const {
        return lhs->pos < rhs->pos;
    }
};

struct Torus
{
    CellPtr enter{};
    CellPtr exit{};
    vector<CellPtr> theMap{};
    set<CellPtr, CellPtrCmp> walkable{};
    long level{0};

    pair<coord, string> processPortals(vector<string>& inputs,
            long x, long y)
    {
        string key{inputs[y][x]};
        const coord initial{x, y};
        coord end{};
        coord portalPosition{};

        long inc{1};
        auto boundcheck = [&](long x, long y){ return x >= 0 && x >=0 && y < inputs.size() && x < inputs[0].size(); };
        if (boundcheck(x, y+inc) && isalpha(inputs[y + inc][x])) {
            key += inputs[y + inc][x];
            end = {x, y + inc};
        } else if(boundcheck(x + inc, y) && isalpha(inputs[y][x + inc])) {
            key += inputs[y][x + inc];
            end = {x + inc, y};
        } else {
            return {};
        }
        const coord diff{end.first - initial.first, end.second - initial.second};
        const coord preInit{initial.first - diff.first, initial.second - diff.second};
        const coord postEnd{end.first + diff.first, end.second + diff.second};
        if (boundcheck(preInit.first, preInit.second) && inputs[preInit.second][preInit.first] == '.')
            portalPosition = preInit;
        else if (boundcheck(postEnd.first, postEnd.second) && inputs[postEnd.second][postEnd.first] == '.')
            portalPosition = postEnd;
        return {portalPosition, key};
    }

    void constructMap(vector<string>& inputs)
    {
        map<string, pair<coord, coord>> portals{};
        for(int i = 0; i != inputs.size(); ++i)
        {
            auto input = inputs[i];
            for(int j = 0; j != input.size(); ++j)
            {
                auto nc = make_shared<Cell>(coord{j, i}, input[j]);
                theMap.push_back(nc);
                if (nc->mark == '.')
                {
                    walkable.insert(nc);
                }
                else if (isalpha(nc->mark))
                {
                    auto p = processPortals(inputs, j, i);
                    if (p.second.empty())
                        continue;
                    auto t = portals.insert({p.second, {p.first, coord{0,0}}});
                    if (!t.second && t.first->second.first != p.first)
                        t.first->second.second = p.first;
                }
            }
        }
        const long width = inputs[0].size();
        const coord midpoint{width/2, inputs.size()/2};
        for(const auto& portal : portals)
        {
            const long offF = portal.second.first.first + portal.second.first.second * width;
            const long offS = portal.second.second.first + portal.second.second.second * width;
            const coord f{abs(portal.second.first.first - midpoint.first),
                          abs(portal.second.first.second - midpoint.second)};
            const coord s{abs(portal.second.second.first - midpoint.first),
                          abs(portal.second.second.second - midpoint.second)};
            if (portal.first == "AA")
            {
                enter = theMap[offF];
                continue;
            }
            else if (portal.first == "ZZ")
            {
                exit = theMap[offF];
                continue;
            }
            if (sqrt(f.first * f.first + f.second * f.second) > sqrt(s.first * s.first + s.second * s.second))
            {
                theMap[offF]->levelChange = -1;
                theMap[offS]->levelChange = 1;
                //theMap[offF]->mark = 'N';
                //theMap[offS]->mark = 'P';
                theMap[offF]->portal = portal.first;
                theMap[offS]->portal = portal.first;
            }
            else
            {
                theMap[offF]->levelChange = 1;
                theMap[offS]->levelChange = -1;
                //theMap[offF]->mark = 'P';
                //theMap[offS]->mark = 'N';
                theMap[offF]->portal = portal.first;
                theMap[offS]->portal = portal.first;
            }
            theMap[offF]->portalCell = theMap[offS];
            //theMap[offF]->mark = 'X';
            theMap[offS]->portalCell = theMap[offF];
            //theMap[offS]->mark = 'X';
        }
        constructConnectivity();
    }

    void constructConnectivity()
    {
        for(auto& cc : walkable)
        {
            auto pos = cc->pos;
            const coord left  {pos.first - 1, pos.second};
            const coord right {pos.first + 1, pos.second};
            const coord top   {pos.first, pos.second - 1};
            const coord bottom{pos.first, pos.second + 1};

            bitset<4> boolmap{};
            for_each(walkable.begin(), walkable.end(),
                [&](auto& el){
                    boolmap.set(0, el->pos == left);
                    boolmap.set(1, el->pos == right);
                    boolmap.set(2, el->pos == top);
                    boolmap.set(3, el->pos == bottom);
                    if (boolmap.any())
                        cc->connectivity.insert(el);
                });
        }
    }

    enum Direction
    {
        head = 0,
        tail = 1
    };

    void calculateDistances()
    {
        map<CellPtr, set<long>> visited{};
        for(const auto& w : walkable)
            visited.insert({w, {}});

        auto cmp = [](CellData const& left, CellData const& right) {
            return left.level > right.level;
        };
        priority_queue<CellData, vector<CellData>, decltype(cmp)> q{cmp};
        q.emplace(enter, 0, 0);

        long totalDistance = -1;
        while(!q.empty())
        {
            CellData current = q.top();
            q.pop();
            if (current.cell == exit && current.level == 0)
            {
                totalDistance = current.distance;
                break;
            }
            // skip visited
            if (auto f = visited.find(current.cell); !f->second.insert(current.level).second)
                continue;
            //indicateMark(current.cell, '*');
            const long newDist{current.distance + 1};
            for(auto const& child : current.cell->connectivity)
                q.emplace(child, current.level, newDist);
            if (current.cell->portalCell && current.level + current.cell->levelChange >= 0)
            {
                if (current.level >= 300)
                    continue;
                //qport.emplace(current.cell->portalCell, current.level + current.cell->levelChange, newDist);
                q.emplace(current.cell->portalCell, current.level + current.cell->levelChange, newDist);
            }
        }
        cout << "The distance to ZZ is " << totalDistance << endl;
        //cout << endl;
        ////long totalSteps = 0;
        //long previousChange = 0;
        ////level = 0;
        //for (pair<CellPtr, long> trav = exit->comingFrom[0]; trav.first != nullptr; )
        //{
        //    const char save = trav.first->mark;
        //    trav.first->mark = '@';
        //    cout << "Step count: " << trav.first->distances[level] << "            \n";
        //    printMap(true);
        //    cout << "\033[" << 1 << "A";
        //    // restore
        //    trav.first->mark = save;

        //    //const long prev = trav.second;
        //    trav = trav.first->comingFrom[level];
        //    if (level != trav.second)
        //    {
        //        level = trav.second;
        //        //cout << "Change level to " << level << " using portal " << trav.first->portal << " done "
        //        //    << trav.first->distances[level] - previousChange << " steps from last tp\n";
        //        previousChange = trav.first->distances[level];
        //    }
        //    std::this_thread::sleep_for(std::chrono::milliseconds(2));
        //}
        //cout << "Step count: " << previousChange << "                     \n";
        //printMap();
    }

    void indicateMark(CellPtr const& cell, char sym = '@')
    {
        const char save = cell->mark;
        cell->mark = sym;
        printMap(true);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        cell->mark = save;
    }

    void printMap(bool getBack = false) const
    {
        // find max y min y
        long minX{}, maxX{}, minY{}, maxY{};
        for_each(theMap.cbegin(), theMap.cend(),
                [&](const auto& el){
                    const coord p = el->pos;
                    minX = min(p.first,  minX);
                    maxX = max(p.first,  maxX);
                    minY = min(p.second, minY);
                    maxY = max(p.second, maxY);
                });
        const long linelen = abs(minX) + abs(maxX) + 1;
        const long collen = abs(minY) + abs(maxY) + 1;
        vector<char> result(linelen*collen, '.');
        const coord offset = {minX, minY};
        for_each(theMap.cbegin(), theMap.cend(),
                [&](const auto& el){
                    coord p{el->pos.first - offset.first, el->pos.second - offset.second};
                    result[p.second*linelen + p.first] = el->mark;
                });
        for(int i = 0; i != result.size(); ++i)
        {
            if (i != 0 && i % linelen == 0)
                cout << "\n";
            cout << result[i];
        }
        cout << "\n";
        // move coursor up
        if (getBack)
            cout << "\033[" << collen << "A";
    }
};

////////////////////////////////////////////////////////////////////////////////
// main
int main()
{
    int N;
    cin >> N;
    string garbage;
    std::getline(cin, garbage);

    vector<string> inputs{};
    for(int i = 0; i != N; ++i)
    {
        string line;
        std::getline(cin, line);
        inputs.push_back(line);
    }

    // part 1
    cout << "Part 1\n";
    Torus tor{};
    tor.constructMap(inputs);
    tor.printMap();
    tor.calculateDistances();
    // part 2
    cout << "Part 2\n";

    return 0;
}

