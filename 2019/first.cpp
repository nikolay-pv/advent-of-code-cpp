#include <iostream>
using namespace std;

long calculateMass(long const& moduleMass)
{
    long result = moduleMass/3 - 2;
    return result < 0 ? 0 : result;
}

int main()
{
    const int N = 100;
    long fuel = 0;
    for(int input = 0; input < N; ++input)
    {
        long mass = 0;
        cin >> mass;
        long additionalFuel = calculateMass(mass);
        // initial value
        fuel += additionalFuel;
        while(additionalFuel > 0)
        {
            // calcualte fuel needed for fuel
            additionalFuel = calculateMass(additionalFuel);
            fuel += additionalFuel;
        }
    }
    cout << fuel << endl;
    return 0;
}

