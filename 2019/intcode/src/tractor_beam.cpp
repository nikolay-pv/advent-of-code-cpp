#include "tractor_beam.hpp"
#include "intcode.hpp"

#include <chrono>
#include <thread>

namespace beam {

Cell::Cell(coord const& p, char m)
    : pos{p}, mark{m}
{ }

std::ostream& operator<<(std::ostream& os, const Cell& obj)
{
    os << "Cell: " << obj.pos.first << " " << obj.pos.second << " with mark " << obj.mark;
    return os;
}


BeamExplorer::BeamExplorer(IntCodeComputer& newBrain, long h, long w)
    : brain{newBrain}, height{h}, width{w}
{
    boundaries = vector<pair<CellPtr, CellPtr>>(height);
    auto probe = [&]()
        {
            //cerr << "Input for pos " << this->currnt.first << " " << this->currnt.second << endl;
            long answ{};
            if (this->nextToPass != nullopt)
            {
                answ = this->nextToPass.value();
                this->nextToPass = nullopt;
                this->previous = this->currnt;
                if (currnt.first + 1 < width) {
                    currnt.first += 1;
                } else {
                    currnt.second += 1;
                    currnt.first = 0;
                }
            } else {
                answ = this->currnt.first;
                this->nextToPass = this->currnt.second;
            }
            return answ;
        };
    static_cast<Input*>(brain.instructionSet[OpcodeInstruction::input].get())->setCallBack(probe);
    auto createCells = [&](long val)
        {
            if (val != 1)
                return;
            this->pullArea += 1;
            auto c = make_shared<Cell>(this->previous, '#');
            //cerr << "Area (pos) " << c->pos.first << " " << c->pos.second << " " << this->pullArea << endl;
            this->theMap.insert(c);
            if (this->boundaries[c->pos.second].first != nullptr)
                this->boundaries[c->pos.second].second = c;
            else
                this->boundaries[c->pos.second].first = c;
        };
    static_cast<Output*>(brain.instructionSet[OpcodeInstruction::output].get())->setCallBack(createCells);
}

void BeamExplorer::probeLocations()
{
    for(int i = 0; i != height; ++i)
    {
        if (i != 0 && boundaries[i - 1].first)
            currnt.first = boundaries[i - 1].first->pos.first;
        for(int j = currnt.first; j != width; ++j)
        {
            auto state = IntCodeComputer::Running;
            long output{-1};
            while (state != IntCodeComputer::Halt)
                std::tie(output, state) = brain.runningLoop(nullopt);
            //printMap(true);
            brain.resetToInit();
            if (i != 0 && boundaries[i - 1].second && j > boundaries[i - 1].second->pos.first && output == 0)
                break;
        }
    }
    printMap();
    cout << "The area of the tractor beam is " << pullArea << endl;
}

void BeamExplorer::findSquarePosition()
{
    const long squareSize = 100;
}

void BeamExplorer::printMap(bool getBack) const
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


} // namespace beam
