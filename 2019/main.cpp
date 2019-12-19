#include <iostream>
#include <vector>
#include <array>
#include <valarray>
#include <map>
#include <set>
#include <list>
#include <memory>
#include <cmath>
#include <numeric>
#include <sstream>

using namespace std;

struct Body
{
    valarray<long> position{0, 0, 0};
    valarray<long> velocity{0, 0, 0};

    Body() = default;
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

std::ostream& operator<<(std::ostream& os, const Body& obj)
{
    assert(obj.position.size() == 3 && obj.velocity.size() == 3 );
    //pos=<x=-1, y=  0, z= 2>, vel=<x= 0, y= 0, z= 0>
    os << "pos= <x=" << obj.position[0] << ", y= " << obj.position[1] << ", z= " << obj.position[2] << ">, ";
    os << "vel= <x=" << obj.velocity[0] << ", y= " << obj.velocity[1] << ", z= " << obj.velocity[2] << ">";
    return os;
}

std::ostream& operator<<(std::ostream& os, const valarray<long>& obj)
{
    os << obj[0] << ", " << obj[1] << ", " << obj[2] << ", " << obj[3];
    return os;
}

struct NbodySystem
{
    enum Coordinate : long
    {
        X = 0,
        Y = 1,
        Z = 2
    };

    string getHashedData(Coordinate coord)
    {
        stringstream ss;
        for_each(bodies.cbegin(), bodies.cend(),
                [&ss, &coord](auto const& el)
                {
                    ss << el.position[static_cast<long>(coord)] << "_";
                    ss << el.velocity[static_cast<long>(coord)] << "_";
                });
        return ss.str();
    }

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

    long calculateTotalEnergy()
    {
        long totalEnergy{};
        for_each(bodies.cbegin(), bodies.cend(),
                [&](const auto& body){
                    totalEnergy += body.calculateTotalEnergy();
                });
        return totalEnergy;
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

    void curryCuriousSimulation()
    {
        printCurrentState(0);
        vector<set<string>> history(3);
        for(int j = 0; j != 3; ++j)
            history[j].insert(getHashedData(static_cast<Coordinate>(j)));
        vector<long> counts(3, 0);
        long iteration = 0;
        long stopper = 3;
        do
        {
            applyGravity();
            applyVelocity();
            ++iteration;
            for(int j = 0; j != 3; ++j)
            {
                if (counts[j] != 0)
                    continue;
                auto [it, isInserted] = history[j].insert(getHashedData(static_cast<Coordinate>(j)));
                if (isInserted)
                    continue;
                counts[j] = iteration;
                --stopper;
            }
        } while(stopper != 0);
        printCurrentState(iteration);
        for_each(counts.cbegin(), counts.cend(), [](const auto& el){ cerr << el << ", ";});
        long result = counts[0];
        for(int i = 1; i != counts.size(); ++i)
            result = lcm(result, counts[i]);
        cout << "lcm is " << result << "\n";
    }

    vector<Body> bodies{};
};

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
        system.bodies.emplace_back(input);
    }
    // part 1
    cout << "Part 1\n";
    const long numSteps = 1000;
    system.currySimulation(numSteps, 1);
    system.printCurrentEnergy(numSteps);
    // part 2
    cout << "Part 2\n";
    system.curryCuriousSimulation();
    return 0;
}

