
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

optional<long> IntCodeComputer::popValueFromCache()
{
    if (cache.size() == 0)
    {
        //cerr << "No input is provided!" << endl;
        return nullopt;
    }
    long val = cache.front();
    cache.erase(cache.begin());
    return val;
};

pair<long, IntCodeComputer::State> IntCodeComputer::runningLoop(long input)
{
    if (state == Halt)
        return { input, state };
    state = Running;
    cache.push_back(input);
    //cerrPrintMemory();
    long offset = 0;
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
        executor->second->passValue(cache);
        executor->second->setModes(parameterMode);
        executor->second->execute(instructionPos, memory);
        executor->second->getValue(result);
        instructionPos += executor->second->paramsLength();
        //cerrPrintMemory();
        //if (result != nullopt)
        //{
        //    state = State::Paused;
        //    break;
        //}
    }
    if (result == nullopt)
    {
        //cout << "Houston, we have a problem!" << endl;
        return {input, Halt};
    }
    if (state != State::Paused)
        state = State::Halt;
    return {result.value(), state};
}

optional<OpcodeInstruction> IntCodeComputer::getCurrentOpcode()
{
    string tmp = to_string(*instructionPos);
    //cerr << "Opcode candidate " << tmp << endl;
    parameterMode.clear();
    // everything but last 2 digits
    if (tmp.length() > 2)
    {
        auto modes = tmp.substr(0, tmp.length() - 2);
        reverse(modes.begin(), modes.end());
        //cerr << "Modes candidate " << modes << endl;
        for(char el : modes)
        //for (auto bit = modes.rbegin(); bit != modes.rend(); ++bit)
        {
            parameterMode.push_back(static_cast<ParamMode>(el));
        }
        //cerr << "Log: setting the mode to " << parameterMode << endl;
    }
    // last 2 digits
    const long code = tmp.length() > 2 ? stol(tmp.substr(tmp.length() - 2, 2)) : *instructionPos;
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

long getParam(vector<long>::iterator begginingOfInstruction,
              vector<long>& computerMemory,
              long offset,
              ParamMode mode)
{
    const long val = *(begginingOfInstruction + offset);
    switch (mode)
    {
        case Positional:
            if (val >= computerMemory.size())
                computerMemory.resize(val, 0);
            return computerMemory[val];
        case Immediate:
            //if (val >= computerMemory.size())
            //    computerMemory.resize(val, 0);
            return val;
        case Relative:
            if (val + IntCodeComputer::relativeBase >= computerMemory.size())
                computerMemory.resize(val + IntCodeComputer::relativeBase, 0);
            return computerMemory[val + IntCodeComputer::relativeBase];
    }
}

long getOutputParam(vector<long>::iterator begginingOfInstruction,
              vector<long>& computerMemory,
              long offset,
              ParamMode mode)
{
    const long val = *(begginingOfInstruction + offset);
    switch (mode)
    {
        case Positional:
            if (val >= computerMemory.size())
                computerMemory.resize(val, 0);
            return val;
        case Immediate:
            assert(mode != Immediate);
            return val;
        case Relative:
            if (val + IntCodeComputer::relativeBase >= computerMemory.size())
                computerMemory.resize(val + IntCodeComputer::relativeBase, 0);
            return val + IntCodeComputer::relativeBase;
    }
}

////////////////////////////////////////////////////////////////////////////////
// CmdExecutors
// Sum
long Sum::paramsLength() { return 4; }
void Sum::execute(vector<long>::iterator begginingOfInstruction,
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
    cerr << "Mode: " << modes << " [" << dst << "] = " << leftVal << " + " << rightVal << endl;
#endif
    computerMemory[dst] = leftVal + rightVal;
}

// Multiply
long Multiply::paramsLength() { return 4; }
void Multiply::execute(vector<long>::iterator begginingOfInstruction,
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
    cerr << "Mode: " << modes << " [" << dst << "] = " << leftVal << " * " << rightVal << endl;
#endif
    computerMemory[dst] = leftVal * rightVal;
}

// Input
void Input::passValue(list<long>& vals)
{
    if (vals.size() == 0)
    {
        cerr << "Got empty list as input!" << endl;
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
void Input::execute(vector<long>::iterator begginingOfInstruction,
                     vector<long>& computerMemory)
{
    const long offset = 1;
    const long dst = getOutputParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
    computerMemory[dst] = inputValue;
#ifdef INTCODEDEBUG
    cerr << "Input; mode: " << modes << " [" << dst << "] = " << inputValue << endl;
#endif
}

// Output
void Output::getValue(optional<long>& val)
{
    val = output;
};
long Output::paramsLength() { return 2; }
void Output::execute(vector<long>::iterator begginingOfInstruction,
                     vector<long>& computerMemory)
{
    const long offset = 1;
    const long val = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
    output = val;
    cout << "Outut: " << val << endl;
}

// Jump_if_true
long Jump_if_true::paramsLength() { return dynamicLength; }
void Jump_if_true::execute(vector<long>::iterator begginingOfInstruction,
                     vector<long>& computerMemory)
{
    dynamicLength = 3;
    long offset = 1;
    const long val = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
    if (val != 0)
    {
        offset = 2;
        const long newpointer = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
        dynamicLength = newpointer - (begginingOfInstruction - computerMemory.begin());
#ifdef INTCODEDEBUG
        cerr << "NewPointer " << newpointer << " diff " << (begginingOfInstruction - computerMemory.begin()) << endl;
#endif
    }
#ifdef INTCODEDEBUG
    cerr << "Mode: " << modes << " [" << dynamicLength << "] = " << val << " != " << 0 << endl;
#endif
}

// Jump_if_false
long Jump_if_false::paramsLength() { return dynamicLength; }
void Jump_if_false::execute(vector<long>::iterator begginingOfInstruction,
                     vector<long>& computerMemory)
{
#ifdef INTCODEDEBUG
    cerr << "Jump_if_false:\t" << *begginingOfInstruction << ", " << *(begginingOfInstruction + 1) << ", " << *(begginingOfInstruction + 2) << endl;
#endif
    dynamicLength = 3;
    long offset = 1;
    const long val = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
    if (val == 0)
    {
        offset = 2;
        const long newpointer = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));
        dynamicLength = newpointer - (begginingOfInstruction - computerMemory.begin());
#ifdef INTCODEDEBUG
        cerr << "NewPointer " << newpointer << " diff " << (begginingOfInstruction - computerMemory.begin()) << endl;
#endif
    }
#ifdef INTCODEDEBUG
    cerr << "Mode: " << modes << " [" << dynamicLength << "] = " << val << " == " << 0 << endl;
#endif
}

// Less_than
long Less_than::paramsLength() { return 4; }
void Less_than::execute(vector<long>::iterator begginingOfInstruction,
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
    cerr << "Mode: " << modes << " [" << dst << "] = " << first << " < " << second << endl;
#endif
}

// Equals
long Equals::paramsLength() { return 4; }
void Equals::execute(vector<long>::iterator begginingOfInstruction,
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
    cerr << "Mode: " << modes << " [" << dst << "] = " << first << " == " << second << endl;
#endif
}

// Rel_base_offset
long Rel_base_offset::paramsLength() { return 2; }
void Rel_base_offset::execute(vector<long>::iterator begginingOfInstruction,
                     vector<long>& computerMemory)
{
    long offset = 1;
    const long first = getParam(begginingOfInstruction, computerMemory, offset, getParamMode(modes, offset));

    IntCodeComputer::relativeBase += first;
#ifdef INTCODEDEBUG
    cerr << "Mode: " << modes << " [ base Offset ] = " << IntCodeComputer::relativeBase << " (" << first << ")" << endl;
#endif
}

//Halt
long Halt::paramsLength() { return 0; }
void Halt::execute(vector<long>::iterator, vector<long>&)
{
    IntCodeComputer::halt();
    cerr << "Halting" << endl;
}
