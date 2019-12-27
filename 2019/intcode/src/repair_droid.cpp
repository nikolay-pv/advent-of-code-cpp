#include "repair_droid.hpp"
#include <memory>
#include <chrono>
#include <thread>

char getGlyph(OutputDroid od)
{
    switch(od) {
    case Wall : return '#';
    case Normal : return '.';
    case Oxygen : return 'O';
    }
}

Cell::Cell(coord const& p, char m)
    : pos{p}, mark{m}
{ }

std::ostream& operator<<(std::ostream& os, const Cell& obj)
{
    os << "Cell: " << obj.pos.first << " " << obj.pos.second << " with mark " << obj.mark;
    os << " return to initial " << obj.returnToInitial << " other ";
    for_each(obj.unVisitedDirs.cbegin(), obj.unVisitedDirs.cend(), [&](const auto& d){ os << d; });
    return os;
}

RepairDroid::RepairDroid(IntCodeComputer& computer)
    : brain{computer}
{
    // create initial position
    coord defpos{0, 0};
    auto cc = make_shared<Cell>(defpos, 'X');
    cc->distance = 0;
    current = cc;
    origin = cc;
    theMap.insert(cc);
    // register mover
    auto moveCommand = [&]()
        {
            //cerr << "current " << current->pos.first << " " << current->pos.second << endl;
            //cerr << "mapsize " << theMap.size() << endl;
            Direction answ;
            if (current->unVisitedDirs.size() == 0)
            {
                //this->emergencyHalt = true;
                //answ = Direction::North;
                //assert(current->returnToInitial != 0);
                answ = current->returnToInitial;
                //cerr << "Returning to inital" << endl;
            }
            else
            {
                answ = current->unVisitedDirs.back();
                current->unVisitedDirs.pop_back();
                //cerr << "unvis size = " << current->unVisitedDirs.size() << endl;
            }
            //if (current->pos.first == -2 && current->pos.second == 4)
            //{
            //    cerr << "At test: " << *current << endl;
            //    cerr << "Coming from: " << currentDirection << " going to: " << answ << endl;
            //}
            currentDirection = answ;
            //cerr << "Ask to move to " << answ << endl;
            if (answ == Direction::Invalid)
                cerr << "Bail out the droid!" << endl;
            return static_cast<long>(answ);
        };
    static_cast<Input*>(brain.instructionSet[OpcodeInstruction::input].get())->setCallBack(moveCommand);
};

CellPtr getNewCell(CellPtr const& current, Direction dir)
{
    coord npos = current->pos;
    Direction returning{Invalid};
    switch(dir) {
    case North : npos.first +=  0; npos.second +=  1; returning = South; break;
    case South : npos.first +=  0; npos.second += -1; returning = North; break;
    case West  : npos.first += -1; npos.second +=  0; returning = East;  break;
    case East  : npos.first +=  1; npos.second +=  0; returning = West;  break;
    case Invalid:
    default:
        cerr << "Underground!" << endl;
    }
    auto nc = make_shared<Cell>(npos, ' ');
    nc->returnToInitial = returning;
    auto& unv = nc->unVisitedDirs;
    unv.erase(find(unv.begin(), unv.end(), returning));
    return nc;
}

void RepairDroid::searchOxygen()
{
    auto state = IntCodeComputer::Running;
    long output{0};
    printMap(true);
    while (state != IntCodeComputer::Halt && !emergencyHalt)
    //long counter = 6;
    //while (--counter != 0)
    {
        //for_each(theMap.cbegin(), theMap.cend(), [](const auto& cc){ cerr << *cc << endl; });
        //cerr << "theMap size " << theMap.size() << endl;
        //cerr << endl;
        std::tie(output, state) = brain.runningLoop(nullopt);
        //cerr << "Current position " << current->pos.first << " " << current->pos.second << endl;
        if (output == OutputDroid::Wall)
        {
            auto nc = getNewCell(current, currentDirection);
            nc->mark = getGlyph(static_cast<OutputDroid>(output));
            nc->unVisitedDirs.clear();
            theMap.insert(nc);
        }
        else if (output == OutputDroid::Normal)
        {
            auto nc = getNewCell(current, currentDirection);
            nc->mark = getGlyph(static_cast<OutputDroid>(output));
            auto ntile = theMap.insert(nc);
            (*ntile.first)->connectivity.insert(current);
            current->connectivity.insert(*ntile.first);
            //if (!ntile.second)
            //    (*ntile.first)->returnToInitial = nc->returnToInitial;
            (*ntile.first)->distance = min((*ntile.first)->distance, current->distance + 1);
            current = *ntile.first;
        }
        else if (output == OutputDroid::Oxygen)
        {
            auto nc = getNewCell(current, currentDirection);
            nc->mark = getGlyph(static_cast<OutputDroid>(output));
            auto ntile = theMap.insert(nc);
            (*ntile.first)->distance = min((*ntile.first)->distance, current->distance + 1);
            (*ntile.first)->connectivity.insert(current);
            current->connectivity.insert(*ntile.first);
            current = *ntile.first;
            oxygen = *ntile.first;
            //break;
        }
        printMap(true);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    printMap(false);
    cout << "Minimal distance from X to O is " << oxygen->distance << endl;
}

namespace {

void runOverNodes(CellPtr const& node)
{
    long newDist{node->distance + 1};
    for_each(node->connectivity.cbegin(), node->connectivity.cend(),
            [&](const auto& cc){
                cc->distance = min(cc->distance, newDist);
            });
    auto tmp{node->connectivity};
    node->connectivity.clear();
    for_each(tmp.cbegin(), tmp.cend(), [&](auto const& cc){ runOverNodes(cc); });
}

} // namespace

void RepairDroid::calculateTimeToFill()
{
    for_each(theMap.begin(), theMap.end(), [&](auto& cc){
            if (cc->mark == '.')
                cc->distance = 1000000;
            });
    oxygen->distance = 0;

    runOverNodes(oxygen);

    long total{};
    for_each(theMap.cbegin(), theMap.cend(), [&](const auto& cc){
            if (cc->mark != '#')
                total = max(total, cc->distance);
            });
    printOxygenSimulation(total);
}

void RepairDroid::printOxygenSimulation(long total) const
{
    vector<vector<CellPtr>> forPrint(total + 1);
    for_each(theMap.cbegin(), theMap.cend(), [&](const auto& cc){
            if (cc->mark != '#')
                forPrint[cc->distance].push_back(cc);
            });
    for(int i = 0; i <= total; ++i)
    {
        for_each(forPrint[i].begin(), forPrint[i].end(), [](auto& cc){ cc->mark='O'; });
        cout << "Time " << i << " m" << endl;
        printMap(true);
        cout << "\033[" << 1 << "A";
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
    cout << "Time " << total << " m" << endl;
    printMap(false);
    cout << "Filling all locations with oxygen will take " << total << endl;
}

void RepairDroid::printMap(bool getBack) const
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
                result[-p.second*linelen + p.first] = (el->pos == current->pos && getBack) ? 'D' : el->mark;
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


