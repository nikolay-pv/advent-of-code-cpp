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
    jump_if_true = 5,
    jump_if_false = 6,
    less_than = 7,
    equals = 8,
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
    case OpcodeInstruction::jump_if_true:
    case OpcodeInstruction::jump_if_false:
    case OpcodeInstruction::less_than:
    case OpcodeInstruction::equals:
        break;
    default :
        cerr << "Ups, got " << value << std::endl;
        return nullopt;
    }
    return static_cast<OpcodeInstruction>(value);
}

class CmdExecutor
{
public:
    long modes = 0;
    virtual void passValue(long& val) { return; };
    virtual void setModes(long val) { modes = val; };
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

    void cerrPrintMemory() const;
};

bool IntCodeComputer::halting = false;

void IntCodeComputer::cerrPrintMemory() const
{
    cerr << "The internal memory of the computer:" << endl;
    for_each(memory.cbegin(), memory.cend(), [](const auto& el){ cerr << el << ", ";});
    cerr << endl;
}

long IntCodeComputer::runningLoop(long input)
{
    cache = input;
    long offset = 0;
    while(!halting)
    {
        instructionPos += offset;
        //cerr << "Instruction position is " << (instructionPos - memory.begin()) << endl;
        auto instrCode = getCurrentOpcode();
        if (instrCode == nullopt)
            break;
        auto executor = instructionSet.find(instrCode.value());
        if (executor == instructionSet.end())
            break;
        //cerrPrintMemory();
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

long getParam(vector<long>::iterator begginingOfInstruction,
              vector<long>& computerMemory,
              long offset,
              bool isImmediateMode)
{
    const long val = *(begginingOfInstruction + offset);
    if (isImmediateMode)
        return val;
    else
        return computerMemory[val];
}

enum ImmediateMode : long
{
    first = 0b01,
    second = 0b10,
    third = 0b100
};

class Sum : public CmdExecutor
{
public:
    long paramsLength() override { return 4; }
    void execute(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) override
    {
        bool isImmediate = modes & ImmediateMode::first;
        const long leftVal = getParam(begginingOfInstruction, computerMemory, 1, isImmediate);
        isImmediate = modes & ImmediateMode::second;
        const long rightVal = getParam(begginingOfInstruction, computerMemory, 2, isImmediate);
        // need position!
        const long dst = getParam(begginingOfInstruction, computerMemory, 3, true);
        //cerr << "Mode: " << modes << " [" << dst << "] = " << leftVal << " + " << rightVal << endl;
        computerMemory[dst] = leftVal + rightVal;
    }
};

class Multiply : public CmdExecutor
{
public:
    long paramsLength() override { return 4; }
    void execute(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) override
    {
        bool isImmediate = modes & ImmediateMode::first;
        const long leftVal = getParam(begginingOfInstruction, computerMemory, 1, isImmediate);
        isImmediate = modes & ImmediateMode::second;
        const long rightVal = getParam(begginingOfInstruction, computerMemory, 2, isImmediate);
        // need position!
        const long dst = getParam(begginingOfInstruction, computerMemory, 3, true);
        //cerr << "Mode: " << modes << " [" << dst << "] = " << leftVal << " * " << rightVal << endl;
        computerMemory[dst] = leftVal * rightVal;
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
    // prevent modification of the cache
    //optional<long> output = nullopt;
public:
    //void passValue(long& val) override { if (output != nullopt) val = output.value(); };
    long paramsLength() override { return 2; }
    void execute(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) override
    {
        const long offset = 1;
        const bool isImmediate = modes & ImmediateMode::first;
        const long val = getParam(begginingOfInstruction, computerMemory, offset, isImmediate);
        cout << "Outut: " << val << endl;
    }
};

class Jump_if_true : public CmdExecutor
{
    long dynamicLength = 3;
public:
    long paramsLength() override { return dynamicLength; }
    void execute(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) override
    {
        dynamicLength = 3;
        const long offset = 1;
        bool isImmediate = modes & ImmediateMode::first;
        const long val = getParam(begginingOfInstruction, computerMemory, offset, isImmediate);
        if (val != 0)
        {
            const bool isImmediate = modes & ImmediateMode::second;
            const long newpointer = getParam(begginingOfInstruction, computerMemory, 2, isImmediate);
            dynamicLength = newpointer - (begginingOfInstruction - computerMemory.begin());
            //cerr << "NewPointer " << newpointer << " diff " << (begginingOfInstruction - computerMemory.begin()) << endl;
        }
        //cerr << "Mode: " << modes << " [" << dynamicLength << "] = " << val << " != " << 0 << endl;
    }
};

class Jump_if_false : public CmdExecutor
{
    long dynamicLength = 3;
public:
    long paramsLength() override { return dynamicLength; }
    void execute(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) override
    {
        dynamicLength = 3;
        const long offset = 1;
        bool isImmediate = modes & ImmediateMode::first;
        const long val = getParam(begginingOfInstruction, computerMemory, offset, isImmediate);
        if (val == 0)
        {
            const bool isImmediate = modes & ImmediateMode::second;
            const long newpointer = getParam(begginingOfInstruction, computerMemory, 2, isImmediate);
            dynamicLength = newpointer - (begginingOfInstruction - computerMemory.begin());
        }
        //cerr << "Mode: " << modes << " [" << dynamicLength << "] = " << val << " == " << 0 << endl;
    }
};

class Less_than : public CmdExecutor
{
public:
    long paramsLength() override { return 4; }
    void execute(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) override
    {
        long offset = 1;
        bool isImmediate = modes & ImmediateMode::first;
        const long first = getParam(begginingOfInstruction, computerMemory, offset, isImmediate);

        offset = 2;
        isImmediate = modes & ImmediateMode::second;
        const long second = getParam(begginingOfInstruction, computerMemory, offset, isImmediate);

        offset = 3;
        // need position!
        const long dst = getParam(begginingOfInstruction, computerMemory, offset, true);

        computerMemory[dst] = first < second ? 1 : 0;
        //cerr << "Mode: " << modes << " [" << dst << "] = " << first << " < " << second << endl;
    }
};

class Equals : public CmdExecutor
{
public:
    long paramsLength() override { return 4; }
    void execute(vector<long>::iterator begginingOfInstruction,
                         vector<long>& computerMemory) override
    {
        long offset = 1;
        bool isImmediate = modes & ImmediateMode::first;
        const long first = getParam(begginingOfInstruction, computerMemory, offset, isImmediate);

        offset = 2;
        isImmediate = modes & ImmediateMode::second;
        const long second = getParam(begginingOfInstruction, computerMemory, offset, isImmediate);

        // need position!
        const long dst = getParam(begginingOfInstruction, computerMemory, 3, true);
        computerMemory[dst] = (first == second) ? 1 : 0;
        //cerr << "Mode: " << modes << " [" << dst << "] = " << first << " == " << second << endl;
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
    // register instructions
    computer.instructionSet.insert({OpcodeInstruction::multiply, CmdExecutorPtr{new Multiply()}});
    computer.instructionSet.insert({OpcodeInstruction::sum, CmdExecutorPtr{new Sum()}});
    computer.instructionSet.insert({OpcodeInstruction::halt, CmdExecutorPtr{new Halt()}});
    computer.instructionSet.insert({OpcodeInstruction::input, CmdExecutorPtr{new Input()}});
    computer.instructionSet.insert({OpcodeInstruction::output, CmdExecutorPtr{new Output()}});
    computer.instructionSet.insert({OpcodeInstruction::jump_if_true, CmdExecutorPtr{new Jump_if_true()}});
    computer.instructionSet.insert({OpcodeInstruction::jump_if_false, CmdExecutorPtr{new Jump_if_false()}});
    computer.instructionSet.insert({OpcodeInstruction::less_than, CmdExecutorPtr{new Less_than()}});
    computer.instructionSet.insert({OpcodeInstruction::equals, CmdExecutorPtr{new Equals()}});

    const long result = computer.runningLoop(5);
    cout << "Return code is " << result << endl;
    return 0;
}

