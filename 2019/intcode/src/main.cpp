#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "intcode.hpp"
//#include "hull_painting_robot.hpp"
//#include "arcade_cabinet.hpp"
//#include "repair_droid.hpp"
//#include "ascii.hpp"
//#include "tractor_beam.hpp"
//#include "springdroid.hpp"
//#include "network.hpp"
#include "santas_ship.hpp"

using namespace std;

int main()
{
    ifstream source{"../input.txt", ios::in};
    string input{};
    getline(source, input);
    const int N{stoi(input)};

    getline(source, input);
    istringstream is{input};
    vector<long> memory;
    for(int input = 0; input != N; ++input)
    {
        long val;
        is >> val;
        memory.push_back(val);
        char separator;
        is >> separator;
    }
    IntCodeComputer computer{"", memory};
    //computer.pausable = true;
    //computer.interrupt = true;

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

    //RepairDroid r2d2{computer};
    //r2d2.searchOxygen();
    //r2d2.calculateTimeToFill();

    //ascii::Ascii tower{computer};
    // Part 1
    //cout << "Part 1\n";
    //tower.constructMap();
    // Part 2
    //cout << "Part 2\n";
    //tower.notifyOthers();

    //beam::BeamExplorer beam{computer, 100, 100};
    //// Part 1
    //cout << "Part 1\n";
    //beam.probeLocations();
    //// Part 2
    //cout << "Part 2\n";
    //beam.findSquarePosition();
    // Day 21
    //spring::SpringDroid kengaroo{computer};
    // Part 1
    //cout << "Part 1\n";
    //kengaroo.run();
    // Part 2
    //cout << "Part 2\n";
    //beam.findSquarePosition();
    // Day 23
    //const long numOfNodes = 50;
    //net::Network lan{numOfNodes, memory};
    //cout << "Part 1 & 2\n";
    //lan.run();
    // Day 25
    santa::SantasShip ship{computer};
    cout << "Part 1\n";
    ship.fly();
    //beam.findSquarePosition();
    return 0;
}

