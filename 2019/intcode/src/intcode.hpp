
#pragma once
#include <iostream>
#include <streambuf>
#include <vector>
#include <memory>
#include <list>
#include <map>

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
    rel_base_offset = 9,
    halt = 99
};

enum ParamMode : char
{
    Positional = '0',
    Immediate = '1',
    Relative = '2',
};

std::ostream& operator<<(std::ostream& os, const vector<ParamMode>& obj);

optional<OpcodeInstruction> castToOpcode(long value);

////////////////////////////////////////////////////////////////////////////////
// CmdExecutor
class CmdExecutor
{
public:
    vector<ParamMode> modes{};
    virtual void passValue(list<long>& vals) { return; };
    virtual void getValue(optional<long>& val) { return; };
    virtual void setModes(vector<ParamMode> val) { modes = std::move(val); };
    virtual long paramsLength() = 0;
    virtual void execute(long begginingOfInstruction,
                         vector<long>& computerMemory) = 0;
    virtual ~CmdExecutor() = default;
};
using CmdExecutorPtr = unique_ptr<CmdExecutor>;

////////////////////////////////////////////////////////////////////////////////
// IntCodeComputer
struct IntCodeComputer
{
    static bool halting;
    static void halt() { halting = true; };
    static long relativeBase;

    IntCodeComputer();
    IntCodeComputer(vector<long> mem);
    IntCodeComputer(string nn, vector<long> mem);

    // config and states
    bool pausable{false};
    bool interrupt{false};
    enum State {
        Running,
        Paused,
        Halt
    };
    State state{Running};

    string name{};

    vector<long> memory{};
    long instructionPos{};
    list<long> cache{};

    map<OpcodeInstruction, CmdExecutorPtr> instructionSet{};
    // parameterMode of the current instruction
    vector<ParamMode> parameterMode{};

    void setPhase(long input) { cache.push_back(input); };
    long getMemoryHead() const { return memory[0]; };

    pair<long, State> runningLoop(optional<long> input);
    optional<OpcodeInstruction> getCurrentOpcode();

    void resetToInit();

private:
    struct InitialState{
        std::vector<long> memory;
    } is;
    void cerrPrintMemory() const;
    void cerrPrintCache() const;
};


////////////////////////////////////////////////////////////////////////////////
// Utilities
ParamMode getParamMode(vector<ParamMode>& modes, long offset);

long getParam(long begginingOfInstruction,
              vector<long>& computerMemory,
              long offset,
              ParamMode mode);

long getOutputParam(long begginingOfInstruction,
              vector<long>& computerMemory,
              long offset,
              ParamMode mode);

////////////////////////////////////////////////////////////////////////////////
// CmdExecutors
class Sum : public CmdExecutor
{
public:
    long paramsLength() override;
    void execute(long begginingOfInstruction,
                 vector<long>& computerMemory) override;
};

class Multiply : public CmdExecutor
{
public:
    long paramsLength() override;
    void execute(long begginingOfInstruction,
                 vector<long>& computerMemory) override;
};

class Input : public CmdExecutor
{
    std::optional<std::function<long()>> callBack{nullopt};
    long inputValue = 0;
public:
    void setCallBack(std::function<long()> nCallBack);
    void removeCallBack();
    void passValue(list<long>& vals) override;
    long paramsLength() override;
    void execute(long begginingOfInstruction,
                 vector<long>& computerMemory) override;
};

class Output : public CmdExecutor
{
    std::optional<std::function<void(long)>> callBack{nullopt};
    // prevent modification of the cache
    optional<long> output = nullopt;
public:
    void setCallBack(std::function<void(long output)> nCallBack);
    void removeCallBack();
    void getValue(optional<long>& val) override;
    long paramsLength() override;
    void execute(long begginingOfInstruction,
                 vector<long>& computerMemory) override;
};

class Jump_if_true : public CmdExecutor
{
    long dynamicLength = 3;
public:
    long paramsLength() override;
    void execute(long begginingOfInstruction,
                 vector<long>& computerMemory) override;
};

class Jump_if_false : public CmdExecutor
{
    long dynamicLength = 3;
public:
    long paramsLength() override;
    void execute(long begginingOfInstruction,
                 vector<long>& computerMemory) override;
};

class Less_than : public CmdExecutor
{
public:
    long paramsLength() override;
    void execute(long begginingOfInstruction,
                 vector<long>& computerMemory) override;
};

class Equals : public CmdExecutor
{
public:
    long paramsLength() override;
    void execute(long begginingOfInstruction,
                 vector<long>& computerMemory) override;
};

class Rel_base_offset : public CmdExecutor
{
public:
    long paramsLength() override;
    void execute(long begginingOfInstruction,
                 vector<long>& computerMemory) override;
};

class Halt : public CmdExecutor
{
public:
    long paramsLength() override;
    void execute(long, vector<long>&) override;
};


void setUpInstructions(map<OpcodeInstruction, CmdExecutorPtr>& instructionSet);
