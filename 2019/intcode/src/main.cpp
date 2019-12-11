#include <iostream>

#include "intcode.hpp"

using namespace std;

using ll = long long;
#define long ll

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

