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

pll processCommand(vector<pll>& result,
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
        result.push_back(start);
        --count;
    }
    return start;
}

vector<pll> processInput(std::string const& input)
{
    vector<pll> result{};
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
    vector<pll> path1{processInput(input)};
    cin >> input;
    vector<pll> path2{processInput(input)};

    vector<pll> intersections{};
    {
        set<pll> spath1{path1.begin(), path1.end()};
        set<pll> spath2{path2.begin(), path2.end()};
        set_intersection(spath1.begin(), spath1.end(),
                         spath2.begin(), spath2.end(),
                         back_inserter(intersections));
    }

    vector<long> steps{};
    transform(intersections.cbegin(), intersections.cend(),
              back_inserter(steps),
              [&](const pll& point) -> long
              {
              return distance(path1.cbegin(), find(path1.cbegin(), path1.cend(), point))
                   + distance(path2.cbegin(), find(path2.cbegin(), path2.cend(), point))
                   // 0 based count
                   + 2;
               });

    std::sort(steps.begin(), steps.end());
    cout << "All steps are: " << endl;
    for_each(steps.cbegin(), steps.cend(),
             [](const auto& el){ cout << el << ", "; });
    cout << endl << "Answer is " << steps[0] << endl;
    return 0;
}

