#include <iostream>
#include <vector>
//#include <array>
//#include <valarray>
//#include <map>
//#include <set>
//#include <list>
//#include <memory>
//#include <cmath>
//#include <numeric>
//#include <sstream>

using namespace std;

struct Pattern
{
    vector<long> pattern{};
    long getCoeff(long row, long pos) const
    {
        if (pos < row)
            return 0;
        pos -= row;
        if (row != 0)
            pos /= ++row;
        return pattern[pos % pattern.size()];
    }
};

void runPhase(vector<long>& input, Pattern const& pattern)
{
    for(int i = 0; i != input.size(); ++i)
    {
        long newVal{};
        //cerr << i << " line: ";
        for(int j = 0; j != input.size(); ++j)
        {
            newVal += input[j] * pattern.getCoeff(i, j);
            //cerr << input[j] << "*" << pattern.getCoeff(i, j) << " + ";
        }
        //cerr << " = " << newVal << endl;
        if (newVal < 0)
            newVal *= -1;
        input[i] = newVal % 10;
    }
}

void runManyPhases(vector<long>& input, Pattern const& pattern, long numPhase)
{
    long phase = 1;
    while(phase <= numPhase)
    {
        runPhase(input, pattern);
        //cout << "After " << phase << " phase: ";
        //for_each(input.cbegin(), input.cend(), [](auto const& el){ cout << el;});
        //cout << endl;
        ++phase;
    }
    cout << "The first 8 number after " << numPhase << " phases ";
    for(int i = 0; i < 8; ++i)
        cout << input[i];
    cout << endl;
}

////////////////////////////////////////////////////////////////////////////////
// main
int main()
{
    int N;
    cin >> N;
    string garbage;
    std::getline(cin, garbage);
    string line;
    std::getline(cin, line);

    vector<long> input{};
    for(const auto& c : line)
        input.push_back(c - '0');

    Pattern pattern{{1,0,-1,0}};

    // part 1
    cout << "Part 1\n";
    runManyPhases(input, pattern, N);
    // part 2
    //cout << "Part 2\n";
    //cout << "The max amout of fuel is " << fuel << " (" << answer << ")\n";
    return 0;
}

