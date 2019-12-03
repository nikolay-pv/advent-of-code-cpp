#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;

enum Direction : char
{
    Right = 'R',
    Up = 'U',
    Left = 'L',
    Down = 'D'
};

using pll = pair<long, long>;
pll operator+(pll lhs, const pll& rhs)
{
    lhs.first += rhs.first;
    lhs.second += rhs.second;
    return lhs;
}

pll processCommand(set<pll>& result,
        pll start,
        char direction, int count)
{
    // x, y
    pll increment{};
    switch(direction) {
        case Direction::Right:
            increment = {1,0};
            break;
        case Direction::Up:
            increment = {0,1};
            break;
        case Direction::Left:
            increment = {-1,0};
            break;
        case Direction::Down:
            increment = {0,-1};
            break;
    }
    while(count > 0)
    {
        start = start + increment;
        result.insert(start);
        --count;
    }
    return start;
}

set<pll> processInput(std::string const& input)
{
    set<pll> result{};
    stringstream ss{input};
    char separator=',';
    pll last{0, 0};
    do{
        char direction;
        ss >> direction;
        int count;
        ss >> count;
        ss >> separator;
        last = processCommand(result, last, direction, count);
        cerr << direction << count << separator;
    } while(ss.good());
    cerr << endl << "Last point: " << last.first << ", " << last.second << endl;
    return result;
}

int main()
{
    int N;
    cin >> N;

    std::string input{};
    cin >> input;
    set<pll> path1{processInput(input)};
    cin >> input;
    set<pll> path2{processInput(input)};

    vector<pll> intersections{};
    set_intersection(path1.begin(), path1.end(),
                     path2.begin(), path2.end(),
                     back_inserter(intersections));

    vector<long> distances{};
    transform(intersections.cbegin(), intersections.cend(),
              back_inserter(distances),
              [](const pll& point) -> long
              { return abs(point.first) + abs(point.second); } );

    std::sort(distances.begin(), distances.end());
    cout << "All distances are: " << endl;
    for_each(distances.cbegin(), distances.cend(),
             [](const auto& el){ cout << el << ", "; });
    cout << endl << "Answer is " << distances[0] << endl;
    return 0;
}

