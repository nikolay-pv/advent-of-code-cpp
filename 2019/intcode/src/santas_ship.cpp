#include "santas_ship.hpp"
#include "intcode.hpp"

#include <chrono>
#include <iomanip>
#include <thread>
#include <fstream>
//#include <queue>
#include <algorithm>

namespace santa {

Cell::Cell(coord const& p, char m)
    : pos{p}, mark{m}
{ }

Cell::Cell(coord const& p, char m, char orig)
    : pos{p}, mark{m}, originalMark{orig}
{ }

std::ostream& operator<<(std::ostream& os, const Cell& obj)
{
    os << "Cell: " << obj.pos.first << " " << obj.pos.second << " with mark " << obj.mark;
    return os;
}


SantasShip::SantasShip(IntCodeComputer& newMap)
    : mapProvider{newMap}, currentPos{new Cell({0, 0}, '@', 'O')}
{
    theMap.insert(currentPos);
    loadHistoryCommands();

    auto probe = [&]()
        {
            if (this->pos == input.size())
            {
                this->processOutput();
                this->printAll();
                this->previousInput = std::move(this->input);
                input.clear();
                this->pos = 0;
                if (commands.size()) {
                    this->input = this->commands.front();
                    this->commands.pop();
                } else {
                    getline(cin, this->input);
                }
                this->input += '\n';
                if (this->input.size() == 1)
                    this->input = previousInput;
                this->processInput();
            }
            return this->input[this->pos++];
        };
    static_cast<Input*>(mapProvider.instructionSet[OpcodeInstruction::input].get())->setCallBack(probe);
    auto createCells = [&](long val)
        {
            output.push_back(val);
        };
    static_cast<Output*>(mapProvider.instructionSet[OpcodeInstruction::output].get())->setCallBack(createCells);
}

void SantasShip::loadHistoryCommands()
{
    ifstream cmds{"../src/commands_25.txt", ios::in};
    if (!cmds.is_open())
    {
        cerr << "Failed to open the file: commands_25.txt\n";
    } else {
        string cmd;
        getline(cmds, cmd);
        int N{stoi(cmd)};
        for(int i = 0; i != N; ++i)
        {
            getline(cmds, cmd);
            cerr << cmd;
            commands.push(cmd);
        }
    }
}

void SantasShip::addNewCell(coord offset)
{
    coord newpos = {currentPos->pos.first + offset.first,
                   currentPos->pos.second + offset.second};
    auto mi = theMap.insert(CellPtr{new Cell(newpos, '\?')});
    //cerr << "inserting to map " << mi.second << endl;
}

CellPtr SantasShip::findCell(coord offset) const
{
    coord newpos = {currentPos->pos.first + offset.first,
                   currentPos->pos.second + offset.second};
    auto mi = theMap.find(CellPtr{new Cell(newpos, '\?')});
    return mi != theMap.end() ? *mi : currentPos;
}

void SantasShip::updateCurrent(CellPtr n)
{
    currentPos->mark = currentPos->originalMark;
    previousPos = currentPos;
    currentPos = n;
    currentPos->mark = '@';
}

constexpr coord north{0, 1};
constexpr coord south{0, -1};
constexpr coord east{1, 0};
constexpr coord west{-1, 0};

void SantasShip::processOutput()
{
    if (auto f = output.find("Doors here lead:"); f != string::npos)
    {
        if (output.find("- north", f) != string::npos)
            addNewCell(north);
        if (output.find("- south", f) != string::npos)
            addNewCell(south);
        if (output.find("- east", f) != string::npos)
            addNewCell(east);
        if (output.find("- west", f) != string::npos)
            addNewCell(west);
    }
    if (auto f = output.find("Items here:"); f != string::npos)
        currentPos->originalMark = 'i';
    if (auto f = output.find("\"Alert!"); f != string::npos)
        updateCurrent(previousPos);
}

void SantasShip::processInput()
{
    if (input == "north\n")
        updateCurrent(findCell(north));
    else if (input == "south\n")
        updateCurrent(findCell(south));
    else if (input == "east\n")
        updateCurrent(findCell(east));
    else if (input == "west\n")
        updateCurrent(findCell(west));

    const auto check = input.substr(0, 4);
    if (check == "take")
        currentPos->originalMark = '.';
    else if (check == "drop")
        currentPos->originalMark = 'i';

    output.clear();
}

// Public
void SantasShip::fly()
{
    auto state = IntCodeComputer::Running;
    long output{-1};
    while (state != IntCodeComputer::Halt)
        std::tie(output, state) = mapProvider.runningLoop(nullopt);
    processOutput();
    printAll();
}

void SantasShip::printMap() const
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
}

void SantasShip::printAll() const
{
    printMap();
    cout << output;
}

} // namespace santa
