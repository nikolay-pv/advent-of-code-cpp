#include <iostream>

#include "intcode.hpp"
#include "hull_painting_robot.hpp"

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
    computer.pausable = true;

    PaintingRobot robot{computer};
    const long painted{robot.paint()};

    cout << "The robot paints at least once " << painted << " panels." << endl;
    return 0;
}

