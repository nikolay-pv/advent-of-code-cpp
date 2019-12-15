#include <iostream>

#include "intcode.hpp"

using namespace std;

int main()
{
    int N;
    cin >> N;

    vector<long> memory;
    for(int input = 0; input != N; ++input)
    {
        long val;
        cin >> val;
        memory.push_back(val);
        char separator;
        cin >> separator;
    }
    IntCodeComputer computer{"", memory};
    // register instructions
    setUpInstructions(computer.instructionSet);

    long output = 2;
    IntCodeComputer::State state{};

    std::tie(output, state) = computer.runningLoop(output);

    cout << "Produced BOOST keycode is " << output << endl;
    //for_each(result.first.cbegin(), result.first.cend(), [](const auto& el){ cout << el << ", "; });
    return 0;
}

