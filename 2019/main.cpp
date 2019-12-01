#include <iostream>
using namespace std;

int calculateMass(int const& moduleMass)
{
    return moduleMass/3 - 2;
}

int main()
{
    const int N = 100;
    long fuel = 0;
    for(int input = 0; input < N; ++input)
    {
        int mass = 0;
        cin >> mass;
        fuel += calculateMass(mass);
    }
    cout << fuel << endl;
    return 0;
}

