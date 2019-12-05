#include <iostream>
#include <vector>
#include <map>
#include <memory>
using namespace std;


enum OpcodeInstruction : long
{
    sum = 1,
    multiply = 2,
    input = 3,
    output = 4,
    halt = 99
};

optional<OpcodeInstruction> castToOpcode(long value)
{
    switch(value) {
    case OpcodeInstruction::sum:
    case OpcodeInstruction::multiply:
    case OpcodeInstruction::halt:
    case OpcodeInstruction::input:
    case OpcodeInstruction::output:
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
    virtual void passValue(long& val) { return; };
    virtual void setModes(long val) { return; };
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

    long cache{};
    long parameterMode{};

    void setMemory() { instructionPos = memory.begin(); };

    long getMemoryHead() const { return memory[0]; };
    long runningLoop(long input);
    optional<OpcodeInstruction> getCurrentOpcode();

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
    cache = input;
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
        executor->second->passValue(cache);
        executor->second->setModes(parameterMode);
        executor->second->execute(instructionPos, memory);
        //executor->second->passValue(cache);
        offset = executor->second->paramsLength();
    }
    if (halting)
        return 0;
    cout << "Houston, we have a problem!" << endl;
    IntCodeComputer::halt();
    return -1;
}

optional<OpcodeInstruction> IntCodeComputer::getCurrentOpcode()
{
    string tmp = to_string(*instructionPos);
    //cerr << "Opcode candidate " << tmp << endl;
    parameterMode = 0;
    // everything but last 2 digits
    if (tmp.length() > 2)
    {
        long counter = 1;
        auto modes = tmp.substr(0, tmp.length() - 2);
        //cerr << "Modes candidate " << modes << endl;
        for (auto bit = modes.rbegin(); bit != modes.rend(); ++bit)
        {
            if (*bit == '1')
                parameterMode += counter;
            counter <<= 1;
        }
        //cerr << "Log: setting the mode to " << parameterMode << endl;
    }
    // last 2 digits
    const long code = tmp.length() > 2 ? stol(tmp.substr(tmp.length() - 2, 2)) : *instructionPos;
    return castToOpcode(code);
}

////////////////////////////////////////////////////////////////////////////////
// CmdExecutors

enum ImmediateMode : long
{
    first = 0b01,
    second = 0b10,
    third = 0b100
};

class Sum : public CmdExecutor
{
    long modes = 0;
    long getLeft(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) const
    {
        const long offset = 1;
        const long val =*(begginingOfInstruction + offset);
        if (modes & ImmediateMode::first)
            return val;
        else
            return computerMemory[val];
    }
    long getRight(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) const
    {
        const long offset = 2;
        const long val = *(begginingOfInstruction + offset);
        if (modes & ImmediateMode::second)
            return val;
        else
            return computerMemory[val];
    }
public:
    void setModes(long val) override { modes = val; };
    long paramsLength() override { return 4; }
    void execute(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) override
    {
        const long lastParam = 3;
        const long destParam = *(begginingOfInstruction + lastParam);
        const long leftVal = getLeft(begginingOfInstruction, computerMemory);
        const long rightVal = getRight(begginingOfInstruction, computerMemory);
        //cerr << "Mode: " << modes << " [" << destParam << "] = " << leftVal << " + " << rightVal << endl;
        computerMemory[destParam] = leftVal + rightVal;
    }
};

class Multiply : public CmdExecutor
{
    long modes = 0;
    long getLeft(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) const
    {
        const long offset = 1;
        const long val = *(begginingOfInstruction + offset);
        if (modes & ImmediateMode::first)
            return val;
        else
            return computerMemory[val];
    }
    long getRight(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) const
    {
        const long offset = 2;
        const long val = *(begginingOfInstruction + offset);
        if (modes & ImmediateMode::second)
            return val;
        else
            return computerMemory[val];
    }
public:
    void setModes(long val) override { modes = val; };
    long paramsLength() override { return 4; }
    void execute(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) override
    {
        const long lastParam = 3;
        const long destParam = *(begginingOfInstruction + lastParam);
        const long leftVal = getLeft(begginingOfInstruction, computerMemory);
        const long rightVal = getRight(begginingOfInstruction, computerMemory);
        //cerr << "Mode: " << modes << " [" << destParam << "] = " << leftVal << " * " << rightVal << endl;
        computerMemory[destParam] = leftVal * rightVal;
    }
};

class Input : public CmdExecutor
{
    long inputValue = 0;
public:
    void passValue(long& val) override { inputValue = val; };
    long paramsLength() override { return 2; }
    void execute(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) override
    {
        const long dst = *(begginingOfInstruction + 1);
        computerMemory[dst] = inputValue;
        cout << "Input: " << inputValue << endl;
    }
};

class Output : public CmdExecutor
{
    long modes = 0;
    // prevent modification of the cache
    //optional<long> output = nullopt;
public:
    //void passValue(long& val) override { if (output != nullopt) val = output.value(); };
    void setModes(long val) override { modes = val; };
    long paramsLength() override { return 2; }
    void execute(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) override
    {
        const long offset = 1;
        long val = *(begginingOfInstruction + offset);
        if (!(modes & ImmediateMode::first))
            val = computerMemory[val];
        cout << "Outut: " << val << endl;
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
    //computer.printMemory();
    // register instructions
    computer.instructionSet.insert({OpcodeInstruction::multiply, CmdExecutorPtr{new Multiply()}});
    computer.instructionSet.insert({OpcodeInstruction::sum, CmdExecutorPtr{new Sum()}});
    computer.instructionSet.insert({OpcodeInstruction::halt, CmdExecutorPtr{new Halt()}});
    computer.instructionSet.insert({OpcodeInstruction::input, CmdExecutorPtr{new Input()}});
    computer.instructionSet.insert({OpcodeInstruction::output, CmdExecutorPtr{new Output()}});

    const long result = computer.runningLoop(1);
    //computer.printMemory();
    cout << "Return code is " << result << endl;
    return 0;
}

