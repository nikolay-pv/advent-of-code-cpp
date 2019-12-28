#include <iostream>
#include <vector>
//#include <array>
//#include <valarray>
//#include <map>
//#include <set>
//#include <list>
//#include <memory>
#include <cmath>
#include <numeric>
//#include <sstream>
#include <optional>

using namespace std;

struct Pattern
{
    vector<long> pattern{};
    long getCoeff(long row, long pos) const
    {
        if (pos < row)
            return 0;
        pos -= row;
        pos /= ++row;
        return pattern[pos % pattern.size()];
    }
};

void runPhase(vector<long>& input, Pattern const& pattern, long offset = 0)
{
    for(int i = 0; i != input.size(); ++i)
    {
        long newVal{};
        //cerr << i << " line: ";
        for(int j = 0; j != input.size(); ++j)
        {
            newVal += input[j] * pattern.getCoeff(i + offset, j + offset) % 10;
            //cerr << input[j] << "*" << pattern.getCoeff(i, j) << " + ";
        }
        //cerr << " = " << newVal << endl;
        input[i] = abs(newVal) % 10;
    }
}

void runManyPhases(vector<long>& input, Pattern const& pattern, long numPhase)
{
    for(long phase = 1; phase <= numPhase; ++phase)
    {
        runPhase(input, pattern);
        //cout << "After " << phase << " phase: ";
        ////for_each(input.cbegin(), input.cend(), [](auto const& el){ cout << el;});
        //for(int i = 0; i != 8; ++i) { cout << input[i]; };
        //cout << endl;
    }
    cout << "The first 8 number after " << numPhase << " phases ";
    for(int i = 0; i < 8; ++i)
        cout << input[i];
    cout << endl;
}

void runSecondManyPhases(vector<long>& input, long sizeMultiplier,
                         long offset, Pattern const& pattern, long numPhase)
{
    const long sz = input.size()*sizeMultiplier - offset;
    vector<long> modifiedinput(sz);
    vector<long> partialSum{};
    partialSum.reserve(sz);
    for(int i = 0; i != sz; ++i)
        modifiedinput[i] = input[(offset + i) % input.size()];
    for(long phase = 1; phase <= numPhase; ++phase)
    {
        partialSum.clear();
        partial_sum(modifiedinput.rbegin(), modifiedinput.rend(), partialSum.begin());
        for(int i = 0; i != sz; ++i)
            modifiedinput[i] = abs(partialSum[sz - i - 1]) % 10;
        //cout << "After " << phase << " phase: ";
        //for(int i = 0; i != 8; ++i) { cout << modifiedinput[i]; };
        //cout << endl;
    }
    cout << "The first 8 offset number after " << numPhase << " phases ";
    for(int i = 0; i != 8; ++i)
        cout << modifiedinput[i];
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
    input.clear();
    // part 2
    cout << "Part 2\n";

    for(const auto& c : line)
        input.push_back(c - '0');
    long offset;
    for(int i = 0; i != 7; ++i)
        offset += input[i]*pow(10, 6 - i);

    runSecondManyPhases(input, 10000, offset, pattern, N);
    //cout << "The value with offset ";
    //for(int i = offset; i != offset + 8; ++i)
    //    cout << input[i];
    //cout << endl;
    return 0;
}

