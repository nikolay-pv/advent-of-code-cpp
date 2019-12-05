#include <iostream>
#include <vector>
#include <map>
#include <memory>
using namespace std;


enum OpcodeInstruction : long
{
    sum = 1,
    multiply = 2,
    halt = 99
};

optional<OpcodeInstruction> castToOpcode(long value)
{
    switch(value) {
    case OpcodeInstruction::sum:
    case OpcodeInstruction::multiply:
    case OpcodeInstruction::halt:
        break;
    default :
        std::cerr << "Ups" << std::endl;
        return nullopt;
    }
    return static_cast<OpcodeInstruction>(value);
}

class CmdExecutor
{
public:
    virtual long paramsLength() = 0;
    virtual void execute(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) = 0;
    virtual ~CmdExecutor() = default;
};
using CmdExecutorPtr = unique_ptr<CmdExecutor>;

////////////////////////////////////////////////////////////////////////////////
// CmdExecutors
struct IntCodeComputer
{
    static bool halting;
    static void halt() { halting = true; };

    map<OpcodeInstruction, CmdExecutorPtr> instructionSet{};
    vector<long> memory{};
    std::vector<long>::iterator instructionPos{};

    void setMemory() { instructionPos = memory.begin(); };

    long getMemoryHead() const { return memory[0]; };
    long runningLoop(long input);
    optional<OpcodeInstruction> getCurrentOpcode() const;

    void printMemory() const;
};

bool IntCodeComputer::halting = false;

void IntCodeComputer::printMemory() const
{
    cout << "The internal memory of the computer:" << endl;
    for_each(memory.cbegin(), memory.cend(), [](const auto& el){ cout << el << ", ";});
    cout << endl;
}

long IntCodeComputer::runningLoop(long input)
{
    long offset = 0;
    while(!halting)
    {
        instructionPos += offset;
        auto instrCode = getCurrentOpcode();
        if (instrCode == nullopt)
            break;
        auto executor = instructionSet.find(instrCode.value());
        if (executor == instructionSet.end())
            break;
        executor->second->execute(instructionPos, memory);
        offset = executor->second->paramsLength();
    }
    if (halting)
        return 0;
    cout << "Houston, we have a problem!" << endl;
    IntCodeComputer::halt();
    return -1;
}

optional<OpcodeInstruction> IntCodeComputer::getCurrentOpcode() const
{
    const long code = *instructionPos;
    return castToOpcode(code);
}

////////////////////////////////////////////////////////////////////////////////
// CmdExecutors
class Sum : public CmdExecutor
{
    enum ParamAddress : int
    {
        first = 1,
        second = 2,
        third = 3
    };
public:
    long paramsLength() override { return 4; }
    void execute(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) override
    {
        const long leftParam = *(begginingOfInstruction + ParamAddress::first);
        const long rightParam = *(begginingOfInstruction + ParamAddress::second);
        const long destParam = *(begginingOfInstruction + ParamAddress::third);
        computerMemory[destParam] = computerMemory[leftParam] + computerMemory[rightParam];
    }
};

class Multiply : public CmdExecutor
{
    enum ParamAddress : int
    {
        first = 1,
        second = 2,
        third = 3
    };
public:
    long paramsLength() override { return 4; }
    void execute(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) override
    {
        const long leftParam = *(begginingOfInstruction + ParamAddress::first);
        const long rightParam = *(begginingOfInstruction + ParamAddress::second);
        const long destParam = *(begginingOfInstruction + ParamAddress::third);
        computerMemory[destParam] = computerMemory[leftParam] * computerMemory[rightParam];
    }
};

class Halt : public CmdExecutor
{
public:
    long paramsLength() override { return 1; }
    void execute(vector<long>::iterator, vector<long>&) override
    {
        IntCodeComputer::halt();
    }
};

////////////////////////////////////////////////////////////////////////////////
// main
int main()
{
    int N;
    cin >> N;

    IntCodeComputer computer{};
    for(int input = 0; input != N; ++input)
    {
        long val;
        cin >> val;
        computer.memory.push_back(val);
        char separator;
        cin >> separator;
    }
    computer.setMemory();

    computer.printMemory();
    // register instructions
    computer.instructionSet.insert({OpcodeInstruction::multiply, CmdExecutorPtr{new Multiply()}});
    computer.instructionSet.insert({OpcodeInstruction::sum, CmdExecutorPtr{new Sum()}});
    computer.instructionSet.insert({OpcodeInstruction::halt, CmdExecutorPtr{new Halt()}});

    const long result = computer.runningLoop(0);
    computer.printMemory();
    cout << "Return code is " << result << endl;
    return 0;
}

