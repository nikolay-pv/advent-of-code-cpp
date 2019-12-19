#include <iostream>

#include "intcode.hpp"
#include "hull_painting_robot.hpp"
#include "arcade_cabinet.hpp"

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

    //PaintingRobot robot{computer};
    //const long painted{robot.paint()};

    //cout << "The robot paints at least once " << painted << " panels." << endl;

    ArcadeCabinet ac{computer};
    ac.run();
    ac.flushOuptut();
    cout << "The arcade cabinet prints at least " << ac.getBlocksCount() << " blocks." << endl;
    return 0;
}

