#include <iostream>
#include <vector>
using namespace std;

enum Opcode : long
{
    sum = 1,
    multiply = 2,
    halt = 99
};

void performCalculation(std::vector<long>::iterator it, std::vector<long>& vinput)
{
    const long leftPos = *(it + 1);
    const long rightPos = *(it + 2);
    const long destPos = *(it + 3);
    switch(*it) {
    case Opcode::sum:
        vinput[destPos] = vinput[leftPos] + vinput[rightPos];
        break;
    case Opcode::multiply:
        vinput[destPos] = vinput[leftPos] * vinput[rightPos];
        break;
    default :
        std::cerr << "Ups" << std::endl;
        break;
    }
}

int main()
{
    int N;
    cin >> N;
    long fuel = 0;
    std::vector<long> vinput(N);
    for(int input = 0; input != N; ++input)
        cin >> vinput[input];
    auto it = vinput.begin();
    while(*it != Opcode::halt)
    {
        if (*it != Opcode::sum && *it != Opcode::multiply)
        {
            cout << "Houston, we have a problem!" << endl;
            break;
        }
        performCalculation(it, vinput);
        it += 4;
    }
    cout << "The result of the programm:" << endl;
    for_each(vinput.cbegin(), vinput.cend(), [](const auto& v){ cout << v << ", ";});
    cout << endl << "The first position:" << endl;
    cout << vinput[0] << endl;
    return 0;
}

