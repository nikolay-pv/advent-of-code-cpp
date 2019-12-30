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

using namespace std;
using coord = std::pair<long, long>;

struct Cell;
using CellPtr = shared_ptr<Cell>;
struct Cell
{
    Cell(coord const& p, char m)
        : pos{p}, mark{m}
    { }

    coord pos{};
    char mark{' '};
    long distance{10000};
    bool visited{false};
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

std::ostream& operator<<(std::ostream& os, const Cell& obj)
{
    os << "Cell: " << obj.pos.first << " " << obj.pos.second << " with mark " << obj.mark;
    return os;
}

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

    pair<coord, string> processPortals(vector<string>& inputs,
            long x, long y)
    {
        string key{inputs[y][x]};
        //inputs[y][x] = '+';
        //cerr << "check xy " << x << " " << y << " " << inputs[y] << endl;
        const coord initial{x, y};
        coord end{};
        coord portalPosition{};

        long inc{1};
        auto boundcheck = [&](long x, long y){ return x >= 0 && x >=0 && y < inputs.size() && x < inputs[0].size(); };
        if (boundcheck(x, y+inc) && isalpha(inputs[y + inc][x])) {
            //cerr << "check xy " << x << " " << y + inc << " " << inputs[y + inc] << endl;
            key += inputs[y + inc][x];
            //inputs[y + inc][x] = '+';
            end = {x, y + inc};
        } else if(boundcheck(x + inc, y) && isalpha(inputs[y][x + inc])) {
            //cerr << "check xy " << x + inc << " " << y << " " << inputs[y] << endl;
            key += inputs[y][x + inc];
            //inputs[y][x + inc] = '+';
            end = {x + inc, y};
        } else if(boundcheck(x, y - inc) && isalpha(inputs[y - inc][x])) {
            //cerr << "check xy " << x << " " << y - inc << " " << inputs[y - inc] << endl;
            key += inputs[y - inc][x];
            //inputs[y - inc][x] = '+';
            end = {x, y - inc};
        } else if(boundcheck(x - inc, y) && isalpha(inputs[y][x - inc])) {
            //cerr << "check xy " << x - inc << " " << y << " " << inputs[y] << endl;
            key += inputs[y][x - inc];
            //inputs[y][x - inc] = '+';
            end = {x - inc, y};
        }
        const coord diff{end.first - initial.first, end.second - initial.second};
        const coord preInit{initial.first - diff.first, initial.second - diff.second};
        const coord postEnd{end.first + diff.first, end.second + diff.second};
        if (boundcheck(preInit.first, preInit.second) && inputs[preInit.second][preInit.first] == '.')
            portalPosition = preInit;
        else if (boundcheck(postEnd.first, postEnd.second) && inputs[postEnd.second][postEnd.first] == '.')
            portalPosition = postEnd;
        //cerr << "Portal point " << portalPosition.first << " " << portalPosition.first << " " << key << endl;
        sort(key.begin(), key.end());
        return {portalPosition, key};
    }

    void constructMap(vector<string>& inputs)
    {
        map<string, pair<coord, coord>> portals{};
        //map<string, CellPtr> portalMapping{};
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
                    auto t = portals.insert({p.second, {p.first, coord{0,0}}});
                    if (!t.second && t.first->second.first != p.first)
                        t.first->second.second = p.first;
                }
            }
        }
        //cerr << "Portals size " << portals.size() << endl;
        const long width = inputs[0].size();
        for(const auto& portal : portals)
        {
            //cerr << portal.first << " " << portal.second.first.first << " " << portal.second.first.second
            //    << " " << portal.second.second.first << " " << portal.second.second.second << endl;
            const long offF = portal.second.first.first + portal.second.first.second * width;
            const long offS = portal.second.second.first + portal.second.second.second * width;
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
            theMap[offF]->connectivity.insert(theMap[offS]);
            //theMap[offF]->mark = 'X';
            theMap[offS]->connectivity.insert(theMap[offF]);
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

    void calculateDistances(CellPtr const& cell)
    {
        if (cell->visited)
            return;
        cell->visited = true;
        const long newDist{cell->distance + 1};
        for(const auto& child : cell->connectivity)
            child->distance = min(child->distance, newDist);
        for(auto& child : cell->connectivity)
            calculateDistances(child);
    }

    void calculateDistances()
    {
        enter->distance = 0;
        calculateDistances(enter);
        cout << "The distance to ZZ is " << exit->distance << endl;
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

