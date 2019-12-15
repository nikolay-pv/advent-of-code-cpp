#include <iostream>
#include <vector>
#include <array>
#include <valarray>
#include <map>
#include <set>
#include <list>
#include <memory>
#include <cmath>
using namespace std;

struct Body
{
    valarray<long> position{0, 0, 0};
    valarray<long> velocity{0, 0, 0};

    explicit Body(const std::string& input)
    {
        size_t equal{}, comma{};
        for(int i = 0; i != 3; ++i)
        {
            equal = input.find('=', comma) + 1;
            comma = input.find(',', equal);
            position[i] = stol(input.substr(equal, comma - equal));
        }
    }

    long calculatePotentialEnergy() const
    {
        return std::abs(position).sum();
    }

    long calculateKineticEnergy() const
    {
        return std::abs(velocity).sum();
    }

    long calculateTotalEnergy() const
    {
        return calculateKineticEnergy() * calculatePotentialEnergy();
    }

};

struct NbodySystem
{

    void applyGravity(Body& first, Body& second)
    {
        for(int coord = 0; coord != 3; ++coord)
        {
            if(second.position[coord] < first.position[coord])
            {
                ++second.velocity[coord];
                --first.velocity[coord];
            }
            else if(second.position[coord] > first.position[coord])
            {
                --second.velocity[coord];
                ++first.velocity[coord];
            }
        }
    }

    void applyGravity()
    {
        for(int i = 0; i != bodies.size(); ++i)
        {
            Body& first = bodies[i];
            for(int j = i + 1; j != bodies.size(); ++j)
                applyGravity(first, bodies[j]);
        }
    }

    void applyVelocity()
    {
        for_each(bodies.begin(), bodies.end(),
                [](auto& body){
                    body.position += body.velocity;
                });
    }

    void printCurrentState(long iteration)
    {
        cout << "After " << iteration << " steps:\n";
        for_each(bodies.cbegin(), bodies.cend(),
                [](const auto& body){
                    cout << body << "\n";
                });
        cout << "\n";
    }

    void printCurrentEnergy(long iteration)
    {
        cout << "Energy after " << iteration << " steps:\n";
        long totalEnergy{};
        cout << "Sum of total energy: ";
        for_each(bodies.cbegin(), bodies.cend(),
                [&](const auto& body){
                    const long tmp = body.calculateTotalEnergy();
                    cout << tmp << " + ";
                    totalEnergy += tmp;
                });
        cout << " = " << totalEnergy << "\n";
    }

    void currySimulation(long forSteps, long printIncrement = 10)
    {
        printCurrentState(0);
        for(long iteration = 1; iteration != forSteps + 1; ++iteration)
        {
            applyGravity();
            applyVelocity();
            if(iteration % printIncrement == 0)
                printCurrentState(iteration);
        }
    }

    vector<Body> bodies{};
};

std::ostream& operator<<(std::ostream& os, const Body& obj)
{
    assert(obj.position.size() == 3 && obj.velocity.size() == 3 );
    //pos=<x=-1, y=  0, z= 2>, vel=<x= 0, y= 0, z= 0>
    os << "pos= <x=" << obj.position[0] << ", y= " << obj.position[1] << ", z= " << obj.position[2] << ">, ";
    os << "vel= <x=" << obj.velocity[0] << ", y= " << obj.velocity[1] << ", z= " << obj.velocity[2] << ">";
    return os;
}

////////////////////////////////////////////////////////////////////////////////
// main
int main()
{
    int N;
    cin >> N;
    char newline;
    cin >> newline;

    NbodySystem system{};
    for(long line = 0; line != N; ++line)
    {
        std::string input;
        std::getline(cin, input);
        system.bodies.push_back(Body(input));
    }
    const long numSteps = 1000;
    system.currySimulation(numSteps, 1);
    system.printCurrentEnergy(numSteps);
    //const long place = 200;
    //const long id = result.id;
    //deployLaser(asteroids, id);
    //auto answer = simulateDistruction(asteroids, place);
    //cout << "The " << place << " asteroid to be vaporized is at " << answer.first << ", " << answer.second << endl;
    //cout << "The required math X*100+Y = " << answer.first*100 + answer.second << endl;
    return 0;
}

