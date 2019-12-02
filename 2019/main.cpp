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

void performSimulation(std::vector<long> computerMemory, long noun, long verb)
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
    cout << "The result of the programm:" << endl;
    for_each(computerMemory.cbegin(), computerMemory.cend(), [](const auto& v){ cout << v << ", ";});
    cout << endl << "The first position:" << endl;
    cout << computerMemory[0] << endl;
}

int main()
{
    int N;
    cin >> N;
    long fuel = 0;
    std::vector<long> computerMemory(N);
    for(int input = 0; input != N; ++input)
        cin >> computerMemory[input];
    long noun = 12, verb = 2;
    performSimulation(computerMemory, noun, verb);
    return 0;
}

