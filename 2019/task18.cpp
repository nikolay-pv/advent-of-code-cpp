#include <iostream>
#include <vector>
//#include <array>
#include <valarray>
#include <map>
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
#include <queue>

using namespace std;
using coord = std::pair<long, long>;

struct Cell;
using CellPtr = shared_ptr<Cell>;
struct Cell
{
    Cell(coord const& p, char m)
        : pos{p}, mark{m}
    { }

    char mark{' '};
    coord pos{};
    std::set<CellPtr> connectivity{};

    friend bool operator >(const Cell& l, const Cell& r)
    { return l.pos.first > r.pos.first && l.pos.second && l.pos.second; }
    friend bool operator <(const Cell& l, const Cell& r)
    { return !(l.pos > r.pos); }
    friend bool operator==(const Cell& l, const Cell& r)
    { return l.pos == r.pos; }
    friend bool operator!=(const Cell& l, const Cell& r)
    { return !(l == r); }
};

struct CellPtrCmp {
    bool operator()(const CellPtr& lhs, const CellPtr& rhs) const {
        return lhs->pos < rhs->pos;
    }
};

std::ostream& operator<<(std::ostream& os, const Cell& obj)
{
    os << "Cell: " << obj.pos.first << " " << obj.pos.second << " with mark " << obj.mark;
    return os;
}


struct KeyChain
{
    std::bitset<26> letters{};
    KeyChain() = default;
    KeyChain(string availableKeys)
    {
        letters.flip();
        for_each(availableKeys.begin(), availableKeys.end(),
                [&](auto& k){
                    letters.flip(k - 'a');
                });
    }
    KeyChain(set<CellPtr, CellPtrCmp> availableKeys)
    {
        letters.flip();
        for_each(availableKeys.begin(), availableKeys.end(),
                [&](auto& k){
                    letters.flip(k->mark - 'a');
                });
    }

    void addKey(char k) { letters.set(k - 'a'); }
    bool hasKey(char k) const { return letters[tolower(k) - 'a']; }
    bool canOpen(char door) const { return letters[door - 'A']; }
    bool allCollected() const { return letters.all(); }

    friend bool operator==(const KeyChain& l, const KeyChain& r)
    { return l.letters == r.letters; }
    friend bool operator <(const KeyChain& l, const KeyChain& r)
    { return l.letters.to_string() < r.letters.to_string(); }
    friend bool operator >(const KeyChain& l, const KeyChain& r)
    { return l.letters.to_string() > r.letters.to_string(); }
};

bool isDoor(CellPtr const& el) { return isalpha(el->mark) && isupper(el->mark); }
bool isKey(CellPtr const& el) { return isalpha(el->mark) && islower(el->mark); }

struct CellData
{
    CellData(vector<CellPtr> c, vector<long> d, KeyChain k, int idx)
        : cells{move(c)}, distances{move(d)}, collectedKeys{k}, active{idx % 4}
    { }
    vector<CellPtr> cells{};
    vector<long> distances{};
    int active{};
    KeyChain collectedKeys{};

    friend bool operator <(const CellData& l, const CellData& r)
    { return l.cells[l.active] == r.cells[r.active]
        ? l.distances[l.active] < r.distances[r.active] : l.cells[l.active] < r.cells[r.active]; }
};

struct Vault
{
    void generateMap(std::vector<string> const& inputs)
    {
        for(int i = 0; i != inputs.size(); ++i)
        {
            long x{};
            for_each(inputs[i].cbegin(), inputs[i].cend(),
                    [&](auto const& ch){
                        coord p{x++, i};
                        auto nc = make_shared<Cell>(p, ch);
                        this->theMap.push_back(nc);
                        if (ch != '#')
                            walkable.push_back(nc);
                        if (ch == '@')
                        {
                            starts.push_back(nc);
                        } else if(isalpha(ch) && islower(ch)) {
                            keys.insert(nc);
                        }
                    });
        }

        for_each(walkable.cbegin(), walkable.cend(),
                [&](const auto& cc){
                    auto pos = cc->pos;
                    const coord left  {pos.first - 1, pos.second};
                    const coord right {pos.first + 1, pos.second};
                    const coord top   {pos.first, pos.second - 1};
                    const coord bottom{pos.first, pos.second + 1};

                    for_each(walkable.cbegin(), walkable.cend(),
                            [&](auto const& el){
                                valarray<bool> boolmap{false, false, false, false};
                                boolmap[0] |= (el->pos == left);
                                boolmap[1] |= (el->pos == right);
                                boolmap[2] |= (el->pos == top);
                                boolmap[3] |= (el->pos == bottom);
                                if (boolmap.max() != false)
                                    cc->connectivity.insert(el);
                            });
                });
    }

    void collectKeys()
    {
        KeyChain masterKey{keys};

        map<coord, set<KeyChain>> visited{};
        queue<CellData> q{};
        long idx = 0;
        q.emplace(starts, vector<long>{0, 0, 0, 0}, masterKey, idx++);

        long totalDistance{};
        while(!q.empty())
        {
            //CellData current = q.top();
            CellData current = q.front();
            q.pop();
            if (current.collectedKeys.allCollected())
            {
                for(auto d : current.distances)
                    totalDistance += d;
                break;
            }
            // skip visited
            if (auto f = visited.find(current.cells[current.active]->pos); f != visited.end())
            {
                auto res = f->second.insert(current.collectedKeys);
                if (!res.second)
                    continue;
            } else {
                visited.insert({current.cells[current.active]->pos, {current.collectedKeys}});
            }
            //indicateMark(current.cells[current.active], '*');
            vector<long> newDist{current.distances};
            newDist[current.active] += 1;
            for(auto const& child : current.cells[current.active]->connectivity)
            {
                vector<CellPtr> newCells{current.cells};
                newCells[current.active] = child;
                if (isDoor(child) && !current.collectedKeys.canOpen(child->mark))
                    continue;
                KeyChain newKeys = current.collectedKeys;
                bool addOthers = false;
                if (isKey(child))
                {
                    newKeys.addKey(child->mark);
                    addOthers = true;
                }
                q.emplace(newCells, newDist, newKeys, current.active);
                if (addOthers)
                {
                    q.emplace(newCells, newDist, newKeys, current.active + 1);
                    q.emplace(newCells, newDist, newKeys, current.active + 2);
                    q.emplace(newCells, newDist, newKeys, current.active + 3);
                }
            }
        }
        //printMap(false);
        cout << "The total number of steps is " << totalDistance << endl;
    }

    void indicateMark(CellPtr const& cell, char sym = '@')
    {
        const char save = cell->mark;
        cell->mark = sym;
        printMap(true);
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
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
        vector<char> result(linelen*collen, ' ');
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

public:
    vector<CellPtr> theMap{};
    vector<CellPtr> walkable{};
    set<CellPtr, CellPtrCmp> keys{};
    vector<CellPtr> starts{};
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
    inputs.reserve(N);
    for(int i = 0; i < N; ++i)
    {
        string line;
        std::getline(cin, line);
        inputs.push_back(line);
    }
    Vault vault{};
    vault.generateMap(inputs);
    // part 1
    cout << "Part 1\n";
    vault.printMap();
    vault.collectKeys();
    // part 2
    cout << "Part 2\n";

    return 0;
}

