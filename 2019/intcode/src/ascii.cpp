#include "ascii.hpp"
#include <memory>
#include <chrono>
#include <thread>
#include <valarray>

namespace ascii {
Cell::Cell(coord const& p, char m)
    : pos{p}, mark{m}
{ }

std::ostream& operator<<(std::ostream& os, const Cell& obj)
{
    os << "Cell: " << obj.pos.first << " " << obj.pos.second << " with mark " << obj.mark;
    return os;
}

Ascii::Ascii(IntCodeComputer& computer)
    : brain{computer}
{
    const string main = "A,B,A,C,A,B,C,B,C,B\n";
    const string A = "R,8,L,10,L,12,R,4\n";
    const string B = "R,8,L,12,R,4,R,4\n";
    const string C = "R,8,L,10,R,8\n";
    const string graphics = "n\n";
    this->inp = main + A + B + C + graphics;
    // register input
    auto input = [&]()
        {
            if (this->last_inp_pos == inp.size())
                cerr << "Too greedy in the input" << endl;
            cerr << "Feeding " << this->last_inp_pos << " " << inp[this->last_inp_pos] << endl;
            return inp[this->last_inp_pos++];
        };
    static_cast<Input*>(brain.instructionSet[OpcodeInstruction::input].get())->setCallBack(input);

    // register mapper
    auto output = [&](long output)
        {
            if (this->lastOutput == newLine && this->lastOutput == output)
                this->stop = true;
            else
                this->lastOutput = output;
            if (output > 127 || this->stop)
            {
                cout << char(output);
                this->dustCollector = output;
                return;
            }
            const char mark = output;
            if (mark == newLine)
            {
                this->width = min(this->width, this->position.first);
                this->position.second += 1;
                this->position.first = 0;
                return;
            }
            auto nc = make_shared<Cell>(this->position, mark);
            this->theMap.push_back(nc);
            if (nc->mark == '#')
                this->scafold.insert(nc);
            this->position.first += 1;
        };
    static_cast<Output*>(brain.instructionSet[OpcodeInstruction::output].get())->setCallBack(output);
};

void Ascii::constructMap()
{
    auto state = IntCodeComputer::Running;
    long output{0};
    while (state != IntCodeComputer::Halt)
        std::tie(output, state) = brain.runningLoop(nullopt);
    findIntersections();
    long alignmentParam{};
    //cerr << "Summing up" << endl;
    for_each(intersections.begin(), intersections.end(),
            [&](auto& el){
                el->mark = 'O';
                alignmentParam += el->pos.first*el->pos.second;
                //cerr << el->pos.first << " * " << el->pos.second << " = " << el->pos.first*el->pos.second << endl;
            });
    //printMap(false);
    //cout << "The sum of the alignment parameters is " << alignmentParam << endl;
}

void Ascii::findIntersections()
{
    for_each(scafold.begin(), scafold.end(),
            [&](auto& cc){
                auto pos = cc->pos;
                const coord left  {pos.first - 1, pos.second};
                const coord right {pos.first + 1, pos.second};
                const coord top   {pos.first, pos.second - 1};
                const coord bottom{pos.first, pos.second + 1};

                valarray<bool> boolmap{false, false, false, false};
                for_each(scafold.begin(), scafold.end(),
                        [&](auto& el){
                            boolmap[0] |= (el->pos == left);
                            boolmap[1] |= (el->pos == right);
                            boolmap[2] |= (el->pos == top);
                            boolmap[3] |= (el->pos == bottom);
                        });
                if (boolmap.min() != false)
                    this->intersections.insert(cc);
            });
}

void Ascii::notifyOthers()
{
    brain.memory[0] = 2;
    constructMap();
    cout << "The robot collected " << dustCollector << " of dust." << endl;
}

void Ascii::printMap(bool getBack) const
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

}// namespace ascii
