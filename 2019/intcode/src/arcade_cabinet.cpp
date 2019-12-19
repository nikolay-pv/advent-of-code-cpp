#include "arcade_cabinet.hpp"
#include <numeric>

//#define ROBOT_DEBUG

char getGlyph(Tile tile)
{
    switch(tile)
    {
        case Empty : return ' ';
        case Wall : return 'W';
        case Block : return '#';
        case HorizontalPaddle : return '-';
        case Ball : return 'o';
    }
}

ArcadeCabinet::ArcadeCabinet(IntCodeComputer& computer)
    : runner{computer}
{};

void ArcadeCabinet::run()
{
    long res = -1;
    IntCodeComputer::State halting = IntCodeComputer::Running;
    vector<long> tmp;
    while(halting != IntCodeComputer::Halt)
    {
        std::tie(res, halting) = runner.runningLoop(nullopt);
        tmp.push_back(res);
        if (tmp.size() == 3)
        {
            theMap.insert({{tmp[0], tmp[1]}, static_cast<Tile>(tmp[2])});
            tmp.clear();
        }
    }
}

long ArcadeCabinet::getBlocksCount() const
{
    return std::accumulate(theMap.cbegin(), theMap.cend(), 0,
            [](long sum, const auto& el){ if (el.second == Tile::Block) ++sum; return sum; });
}

void ArcadeCabinet::flushOuptut() const
{
    // find max y min y
    long minX{}, maxX{}, minY{}, maxY{};
    for_each(theMap.cbegin(), theMap.cend(),
            [&](const auto& el){
                const pair<long, long> p = el.first;
                minX = min(p.first,  minX);
                maxX = max(p.first,  maxX);
                minY = min(p.second, minY);
                maxY = max(p.second, maxY);
            });
    const long linelen = abs(minX) + abs(maxX) + 1;
    const long collen = abs(minY) + abs(maxY) + 1;
    vector<char> result(linelen*collen, '.');
    const pair<long, long> offset = {minX, maxY};
    for_each(theMap.cbegin(), theMap.cend(),
            [&](const auto& el){
                pair<long, long> p{el.first.first - offset.first, el.first.second - offset.second};
                result[-p.second*linelen + p.first] = getGlyph(el.second);
            });
    for(int i = 0; i != result.size(); ++i)
    {
        if (i != 0 && i % linelen == 0)
            cout << "\n";
        cout << result[i];
    }
    cout << "\n";
}

