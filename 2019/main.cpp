#include <iostream>
#include <vector>
using namespace std;

enum OpcodeInstruction : long
{
    sum = 1,
    multiply = 2,
    halt = 99
};

enum ParamAddress : int
{
    first = 1,
    second = 2,
    third = 3
};

void performCalculation(std::vector<long>::iterator begginingOfInstruction,
                        std::vector<long>& computerMemory)
{
    const long leftParam = *(begginingOfInstruction + ParamAddress::first);
    const long rightParam = *(begginingOfInstruction + ParamAddress::second);
    const long destParam = *(begginingOfInstruction + ParamAddress::third);
    switch(*begginingOfInstruction) {
    case OpcodeInstruction::sum:
        computerMemory[destParam] = computerMemory[leftParam] + computerMemory[rightParam];
        break;
    case OpcodeInstruction::multiply:
        computerMemory[destParam] = computerMemory[leftParam] * computerMemory[rightParam];
        break;
    default :
        std::cerr << "Ups" << std::endl;
        break;
    }
}

long performSimulation(std::vector<long> computerMemory, long noun, long verb)
{
    computerMemory[1] = noun;
    computerMemory[2] = verb;
    auto instrctionPointer = computerMemory.begin();
    const int numOfValuesInInstruction = 4;
    while(*instrctionPointer != OpcodeInstruction::halt)
    {
        if (*instrctionPointer != OpcodeInstruction::sum && *instrctionPointer != OpcodeInstruction::multiply)
        {
            cout << "Houston, we have a problem!" << endl;
            break;
        }
        performCalculation(instrctionPointer, computerMemory);
        instrctionPointer += numOfValuesInInstruction;
    }
    //cout << "The result of the programm:" << endl;
    //for_each(computerMemory.cbegin(), computerMemory.cend(), [](const auto& v){ cout << v << ", ";});
    //cout << endl << "The first position:" << endl;
    //cout << computerMemory[0] << endl;
    return computerMemory[0];
}

int main()
{
    int N;
    cin >> N;
    long fuel = 0;
    std::vector<long> computerMemory(N);
    for(int input = 0; input != N; ++input)
        cin >> computerMemory[input];
    const long target = 19690720;
    long noun = 13, verb = 2;
    long result = performSimulation(computerMemory, noun, verb);
    // first increase noun
    while(result < target)
        result = performSimulation(computerMemory, ++noun, verb);
    if (result != target)
    {
        // decrease noun as we are too high and increase verb as we will be too low
        result = performSimulation(computerMemory, --noun, ++verb);
        // then increase verb
        while(result < target)
            result = performSimulation(computerMemory, noun, ++verb);
    }
    cout << "Noun is " << noun << endl;
    cout << "Verb is " << verb << endl;
    cout << "Result is " << result << endl;
    cout << "Target is " << target << endl;
    cout << "Answer is " << 100 * noun + verb << endl;
    return 0;
}

