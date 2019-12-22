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

void ArcadeCabinet::insertDate(std::vector<long>& data)
{
    if(data[0] == -1)
    {
        score = data[2];
        return;
    }
    const pair<pair<long, long>, Tile> candidate{{data[0], data[1]}, static_cast<Tile>(data[2])};
    theMap.insert_or_assign(candidate.first, candidate.second);
    switch(candidate.second)
    {
        default: break;
        case Ball: ballPos = candidate.first; break;
        case HorizontalPaddle: paddlePos = candidate.first; break;
    }
}

void ArcadeCabinet::run(optional<long> coins)
{
    if (coins != nullopt)
        runner.memory[0] = coins.value();
    long res = -1;
    IntCodeComputer::State halting = IntCodeComputer::Running;
    vector<long> tmp;
    optional<long> input{};
    while(halting != IntCodeComputer::Halt)
    {
        if (tmp.size() == 3)
        {
            insertDate(tmp);
            input = 0;
            tmp.clear();
        }
        if (coins != nullopt)
            flushOuptut(halting != IntCodeComputer::Halt);
        std::tie(res, halting) = runner.runningLoop(input);
        tmp.push_back(res);
        input.reset();
    }
    flushOuptut(false);
}

long ArcadeCabinet::getBlocksCount() const
{
    return std::accumulate(theMap.cbegin(), theMap.cend(), 0,
            [](long sum, const auto& el){ if (el.second == Tile::Block) ++sum; return sum; });
}

void ArcadeCabinet::flushOuptut(bool getBack) const
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
    const pair<long, long> offset = {minX, minY};
    for_each(theMap.cbegin(), theMap.cend(),
            [&](const auto& el){
                pair<long, long> p{el.first.first - offset.first, el.first.second - offset.second};
                result[p.second*linelen + p.first] = getGlyph(el.second);
            });
    cout << "Score: " << score << "\n";
    for(int i = 0; i != result.size(); ++i)
    {
        if (i != 0 && i % linelen == 0)
            cout << "\n";
        cout << result[i];
    }
    cout << "\n";
    // move coursor up
    if (getBack)
        cout << "\033[" << collen + 1<< "A";
}

