
#include "intcode.hpp"

//#define INTCODEDEBUG 1

std::ostream& operator<<(std::ostream& os, const vector<ParamMode>& obj)
{
    for_each(obj.cbegin(), obj.cend(), [&os](const auto& el){ os << static_cast<char>(el); });
    return os;
}

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
    case OpcodeInstruction::rel_base_offset:
        break;
    default :
        cerr << "Ups, got " << value << std::endl;
        return nullopt;
    }
    return static_cast<OpcodeInstruction>(value);
}


////////////////////////////////////////////////////////////////////////////////
// IntCodeComputer
bool IntCodeComputer::halting = false;
long IntCodeComputer::relativeBase = 0;

IntCodeComputer::IntCodeComputer()
{
    halting = false;
    setUpInstructions(this->instructionSet);
};
IntCodeComputer::IntCodeComputer(vector<long> mem)
    : memory{std::move(mem)}
{
    halting = false;
    instructionPos = 0;
    is.memory = memory;
    setUpInstructions(this->instructionSet);
};
IntCodeComputer::IntCodeComputer(string nn, vector<long> mem)
    : IntCodeComputer(std::move(mem))
{
    name = std::move(nn);
};

void IntCodeComputer::resetToInit()
{
    halting = false;
    for(long i = 0; i != is.memory.size(); ++i)
        memory[i] = is.memory[i];
    instructionPos = 0;
};


void IntCodeComputer::cerrPrintMemory() const
{
    cerrPrintCache();
    cerr << "The internal memory of the computer [" << name << "] :" << endl;
    for_each(memory.cbegin(), memory.cend(), [](const auto& el){ cerr << el << ", ";});
    cerr << endl;
}

void IntCodeComputer::cerrPrintCache() const
{
    cerr << "The internal cache of the computer [" << name << "] :" << endl;
    for_each(cache.cbegin(), cache.cend(), [](const auto& el){ cerr << el << ", ";});
    cerr << endl;
}

pair<long, IntCodeComputer::State> IntCodeComputer::runningLoop(optional<long> input)
{
    if (state == Halt)
        return { input == nullopt ? -42 : input.value(), state };
    state = Running;
    if (input != nullopt)
        cache.push_back(input.value());
    //cerrPrintMemory();
    //long offset = 0;
    optional<long> result = nullopt;
    while(!halting)
    {
        //cerr << "Instruction position is " << (instructionPos - memory.begin()) << endl;
        auto instrCode = getCurrentOpcode();
        if (instrCode == nullopt)
            break;
        auto executor = instructionSet.find(instrCode.value());
        if (executor == instructionSet.end())
            break;
        //cerr << "Inside runner before passing to executor" << endl;
        if (pausable && instrCode == OpcodeInstruction::input && interrupt)
            return {-4242, Paused};
        executor->second->passValue(cache);
        executor->second->setModes(parameterMode);
        executor->second->execute(instructionPos, memory);
        executor->second->getValue(result);
        instructionPos += executor->second->paramsLength();
        //cerrPrintMemory();
        if (pausable && result != nullopt)
        {
            state = State::Paused;
            break;
        }
    }
    if (result == nullopt)
    {
        //cout << "Houston, we have a problem!" << endl;
        return {input == nullopt ? -42 : input.value(), Halt};
    }
    if (state != State::Paused)
        state = State::Halt;
    return {result.value(), state};
}

optional<OpcodeInstruction> IntCodeComputer::getCurrentOpcode()
{
    string tmp = to_string(memory[instructionPos]);
    //cerr << "Opcode candidate " << tmp << endl;
    parameterMode.clear();
    // everything but last 2 digits
    if (tmp.length() > 2)
    {
        auto modes = tmp.substr(0, tmp.length() - 2);
        reverse(modes.begin(), modes.end());
        //cerr << "Modes candidate " << modes << endl;
        for(char el : modes)
        {
            parameterMode.push_back(static_cast<ParamMode>(el));
        }
        //cerr << "Log: setting the mode to " << parameterMode << endl;
    }
    // last 2 digits
    const long code = tmp.length() > 2 ? stol(tmp.substr(tmp.length() - 2, 2)) : memory[instructionPos];
    //cerr << "Log: try to cast the code " << code << endl;
    return castToOpcode(code);
}


////////////////////////////////////////////////////////////////////////////////
// Utilities
ParamMode getParamMode(vector<ParamMode>& modes, long offset)
{
    if (offset - 1 >= modes.size())
        return Positional;
    return modes[offset - 1];
}

long getParam(long begginingOfInstruction,
              vector<long>& computerMemory,
              long offset,
              ParamMode mode)
{
    const long val = computerMemory[begginingOfInstruction + offset];
    switch (mode)
    {
        case Positional:
            if (val >= computerMemory.size())
                computerMemory.resize(val + 1, 0);
#ifdef INTCODEDEBUG
            cerr << "\tGetParam. mode: " << char{mode} << " position: " << val << " value: " << computerMemory[val] << endl;
#endif
            return computerMemory[val];
        case Immediate:
            //if (val >= computerMemory.size())
            //    computerMemory.resize(val, 0);
#ifdef INTCODEDEBUG
            cerr << "\tGetParam. mode: " << char{mode} << " position: " << val << " value: " << val << endl;
#endif
            return val;
        case Relative:
            if (val + IntCodeComputer::relativeBase >= computerMemory.size())
                computerMemory.resize(val + IntCodeComputer::relativeBase + 1, 0);
#ifdef INTCODEDEBUG
            cerr << "\tGetParam. mode: " << char{mode}
                << " position: " << val + IntCodeComputer::relativeBase
                << " value: " << computerMemory[val + IntCodeComputer::relativeBase] << endl;
#endif
            return computerMemory[val + IntCodeComputer::relativeBase];
    }
}

long getOutputParam(long begginingOfInstruction,
              vector<long>& computerMemory,
              long offset,
              ParamMode mode)
{
    const long val = computerMemory[begginingOfInstruction + offset];
    switch (mode)
    {
        case Positional:
            if (val >= computerMemory.size())
                computerMemory.resize(val + 1, 0);
#ifdef INTCODEDEBUG
            cerr << "\tGetOutputParam. mode: " << char{mode} << " position: " << val << endl;
#endif
            return val;
        case Immediate:
            assert(mode != Immediate);
            return val;
#ifdef INTCODEDEBUG
            cerr << "\tGetOutputParam. mode: " << char{mode} << " position: " << val << endl;
#endif
        case Relative:
            if (val + IntCodeComputer::relativeBase >= computerMemory.size())
                computerMemory.resize(val + IntCodeComputer::relativeBase + 1, 0);
#ifdef INTCODEDEBUG
            cerr << "\tGetOutputParam. mode: " << char{mode} << " position: " << val + IntCodeComputer::relativeBase << endl;
#endif
            return val + IntCodeComputer::relativeBase;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CmdExecutors
// Sum
long Sum::paramsLength() { return 4; }
void Sum::execute(long begginingOfInstruction,
                     vector<long>& computerMemory)
{
    long offset = 1;
    const long leftVal = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
    offset = 2;
    const long rightVal = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
    // need position!
    offset = 3;
    const long dst = getOutputParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
#ifdef INTCODEDEBUG
    cerr << "Sum Mode: " << modes << " [" << dst << "] = " << leftVal << " + " << rightVal << endl;
#endif
    //if (dst > computerMemory.size())
    //{
    //    cerr << "the size is " << computerMemory.size() << " dst is " << dst << endl;
    //    cerr << "Sum Mode: " << modes << " [" << dst << "] = " << leftVal << " + " << rightVal << endl;
    //}
    computerMemory[dst] = leftVal + rightVal;
}

// Multiply
long Multiply::paramsLength() { return 4; }
void Multiply::execute(long begginingOfInstruction,
                     vector<long>& computerMemory)
{
    long offset = 1;
    const long leftVal = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
    offset = 2;
    const long rightVal = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
    // need position!
    offset = 3;
    const long dst = getOutputParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
#ifdef INTCODEDEBUG
    cerr << "Mul Mode: " << modes << " [" << dst << "] = " << leftVal << " * " << rightVal << endl;
#endif
    computerMemory[dst] = leftVal * rightVal;
}

// Input
void Input::setCallBack(std::function<long()> nCallBack)
{
    callBack = nCallBack;
}

void Input::removeCallBack()
{
    callBack = nullopt;
}

void Input::passValue(list<long>& vals)
{
    if (callBack != nullopt)
    {
        inputValue = callBack.value()();
#ifdef INTCODEDEBUG
        cerr << "Calling the function from input!" << endl;
        assert(vals.size() == 0);
#endif
    }
    else if (vals.size() == 0)
    {
#ifdef INTCODEDEBUG
        cerr << "Got empty list as input!" << endl;
#endif
        inputValue = 0;
    }
    else
    {
#ifdef INTCODEDEBUG
        cerr << "Input executor: changing " << inputValue << " to " << vals.front() << endl;
#endif
        inputValue = vals.front();
        vals.erase(vals.begin());
    }
};
long Input::paramsLength() { return 2; }
void Input::execute(long begginingOfInstruction,
                     vector<long>& computerMemory)
{
    const long offset = 1;
    const long dst = getOutputParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
    computerMemory[dst] = inputValue;
#ifdef INTCODEDEBUG
    cerr << ">> Input Mode: " << modes << " [" << dst << "] = " << inputValue << endl;
#endif
}

// Output
void Output::setCallBack(std::function<void(long)> nCallBack)
{
    callBack = nCallBack;
}

void Output::removeCallBack()
{
    callBack = nullopt;
}

void Output::getValue(optional<long>& val)
{
    if (callBack != nullopt && output != nullopt)
    {
        callBack.value()(output.value());
#ifdef INTCODEDEBUG
        cerr << "Calling the function from output!" << endl;
#endif
    }
    val = output;
};
long Output::paramsLength() { return 2; }
void Output::execute(long begginingOfInstruction,
                     vector<long>& computerMemory)
{
    const long offset = 1;
    const long val = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
    output = val;
#ifdef INTCODEDEBUG
    cerr << "Output: " << val << endl;
#endif
}

// Jump_if_true
long Jump_if_true::paramsLength() { return dynamicLength; }
void Jump_if_true::execute(long begginingOfInstruction,
                     vector<long>& computerMemory)
{
    dynamicLength = 3;
    long offset = 1;
    const long val = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
    if (val != 0)
    {
        offset = 2;
        const long newpointer = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
        dynamicLength = newpointer - begginingOfInstruction;
#ifdef INTCODEDEBUG
        cerr << "JiT NewPointer " << newpointer << " diff " << begginingOfInstruction << endl;
#endif
    }
#ifdef INTCODEDEBUG
    cerr << "JiT Mode: " << modes << " [" << dynamicLength << "] = " << val << " != " << 0 << endl;
#endif
}

// Jump_if_false
long Jump_if_false::paramsLength() { return dynamicLength; }
void Jump_if_false::execute(long begginingOfInstruction,
                     vector<long>& computerMemory)
{
    dynamicLength = 3;
    long offset = 1;
    const long val = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
    if (val == 0)
    {
        offset = 2;
        const long newpointer = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
        dynamicLength = newpointer - begginingOfInstruction;
#ifdef INTCODEDEBUG
        cerr << "JiF NewPointer " << newpointer << " diff " << begginingOfInstruction << endl;
#endif
    }
#ifdef INTCODEDEBUG
    cerr << "JiF Mode: " << modes << " [" << dynamicLength << "] = " << val << " == " << 0 << endl;
#endif
}

// Less_than
long Less_than::paramsLength() { return 4; }
void Less_than::execute(long begginingOfInstruction,
                     vector<long>& computerMemory)
{
    long offset = 1;
    const long first = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));

    offset = 2;
    const long second = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));

    offset = 3;
    // need position!
    const long dst = getOutputParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));

    computerMemory[dst] = first < second ? 1 : 0;
#ifdef INTCODEDEBUG
    cerr << "Lt  Mode: " << modes << " [" << dst << "] = " << first << " < " << second << endl;
#endif
}

// Equals
long Equals::paramsLength() { return 4; }
void Equals::execute(long begginingOfInstruction,
                     vector<long>& computerMemory)
{
    long offset = 1;
    const long first = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));

    offset = 2;
    const long second = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));

    // need position!
    offset = 3;
    const long dst = getOutputParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
    computerMemory[dst] = (first == second) ? 1 : 0;
#ifdef INTCODEDEBUG
    cerr << "Eq  Mode: " << modes << " [" << dst << "] = " << first << " == " << second << endl;
#endif
}

// Rel_base_offset
long Rel_base_offset::paramsLength() { return 2; }
void Rel_base_offset::execute(long begginingOfInstruction,
                     vector<long>& computerMemory)
{
    long offset = 1;
    const long first = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));

    IntCodeComputer::relativeBase += first;
#ifdef INTCODEDEBUG
    cerr << "BaO Mode: " << modes << " [ base Offset ] = " << IntCodeComputer::relativeBase << " (" << first << ")" << endl;
#endif
}

//Halt
long Halt::paramsLength() { return 0; }
void Halt::execute(long, vector<long>&)
{
    IntCodeComputer::halt();
#ifdef INTCODEDEBUG
    cerr << "Halting" << endl;
#endif
}


void setUpInstructions(map<OpcodeInstruction, CmdExecutorPtr>& instructionSet)
{
    instructionSet.insert({OpcodeInstruction::multiply, CmdExecutorPtr{new Multiply()}});
    instructionSet.insert({OpcodeInstruction::sum, CmdExecutorPtr{new Sum()}});
    instructionSet.insert({OpcodeInstruction::halt, CmdExecutorPtr{new Halt()}});
    instructionSet.insert({OpcodeInstruction::input, CmdExecutorPtr{new Input()}});
    instructionSet.insert({OpcodeInstruction::output, CmdExecutorPtr{new Output()}});
    instructionSet.insert({OpcodeInstruction::jump_if_true, CmdExecutorPtr{new Jump_if_true()}});
    instructionSet.insert({OpcodeInstruction::jump_if_false, CmdExecutorPtr{new Jump_if_false()}});
    instructionSet.insert({OpcodeInstruction::less_than, CmdExecutorPtr{new Less_than()}});
    instructionSet.insert({OpcodeInstruction::equals, CmdExecutorPtr{new Equals()}});
    instructionSet.insert({OpcodeInstruction::rel_base_offset, CmdExecutorPtr{new Rel_base_offset()}});
}

