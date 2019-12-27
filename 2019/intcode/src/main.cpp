#include <iostream>
#include <cmath>

#include "intcode.hpp"
//#include "hull_painting_robot.hpp"
//#include "arcade_cabinet.hpp"
#include "repair_droid.hpp"

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

    //ArcadeCabinet ac{computer};
    //auto player = [&ac]()
    //        {
    //            const long diff{ac.ballPos.first - ac.paddlePos.first};
    //            //cerr << "Diff: " << diff << endl;
    //            return diff == 0 ? 0 : std::copysign(1, diff);
    //        };
    //static_cast<Input*>(computer.instructionSet[OpcodeInstruction::input].get())->setCallBack(player);
    //const long numberOfCoins = 2;
    //ac.run(numberOfCoins);
    //cout << "The arcade cabinet prints at least " << ac.getBlocksCount() << " blocks." << endl;

    RepairDroid r2d2{computer};
    r2d2.searchOxygen();
    r2d2.calculateTimeToFill();
    return 0;
}

