#include <iostream>

#include "intcode.hpp"

using namespace std;

using ll = long long;
#define long ll

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
    setUpInstructions(computer.instructionSet);

    long output = 1;

    auto [out, state] = computer.runningLoop(output);
    output = out;

    cout << "Produced BOOST keycode is " << output << endl;
    //for_each(result.first.cbegin(), result.first.cend(), [](const auto& el){ cout << el << ", "; });
    return 0;
}

