#include "springdroid.hpp"
#include "intcode.hpp"

#include <chrono>
#include <thread>
#include <queue>

namespace spring {

Cell::Cell(coord const& p, char m)
    : pos{p}, mark{m}
{ }

std::ostream& operator<<(std::ostream& os, const Cell& obj)
{
    os << "Cell: " << obj.pos.first << " " << obj.pos.second << " with mark " << obj.mark;
    return os;
}

SpringDroid::SpringDroid(IntCodeComputer& newBrain)
    : brain{newBrain}
{
    auto probe = [&]()
        {
            static long inc{};
            string program =
"NOT C J\n"
"AND D J\n"
"AND H J\n"
"NOT B T\n"
"AND D T\n"
"OR T J\n"
"NOT A T\n"
"OR T J\n"
"RUN\n"
;
            //cerr << "Input for pos " << inc << " " << program[inc] << endl;
            return program[inc++];
        };
    static_cast<Input*>(brain.instructionSet[OpcodeInstruction::input].get())->setCallBack(probe);
    auto createCells = [&](long val)
        {
            if (!isalpha(val))
            {
                this->damage = val;
                return;
            }
            cerr << char(val);
        };
    static_cast<Output*>(brain.instructionSet[OpcodeInstruction::output].get())->setCallBack(createCells);
}


void SpringDroid::run()
{
    auto state = IntCodeComputer::Running;
    long output{-1};
    while (state != IntCodeComputer::Halt)
        std::tie(output, state) = brain.runningLoop(nullopt);
    cout << "\nAmount of damage to the hull is " << damage << endl;
    if (!theMap.empty())
        printMap();
    //brain.resetToInit();
}

void SpringDroid::printMap(bool getBack) const
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
    const coord offset = {minX, maxY};
    for_each(theMap.cbegin(), theMap.cend(),
            [&](const auto& el){
                coord p{el->pos.first - offset.first, el->pos.second - offset.second};
                result[-p.second*linelen + p.first] = el->mark;
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


} // namespace spring
