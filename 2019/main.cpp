#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <list>
#include <memory>
#include <cmath>
using namespace std;

using pll = pair<long, long>;
using pdd = pair<double, double>;

struct Asteroid
{
    long id{-1};
    pll position{};
    long inRange{-1};
};

// r and phi
pair<double, double> polarCoordinates(pll position)
{
    double x = position.first;
    double y = position.second;
    double phi = atan2(y,x);
    double r = sqrt(y * y + x * x);
    return {r, phi};
}

std::ostream& operator<<(std::ostream& os, const Asteroid& obj)
{
    os << "Asteroid: " << obj.id << " in position " << obj.position.first << ","
       << obj.position.second << " and in Range: " << obj.inRange;
    return os;
}

static long fsize = 0;

pll transformCoordinates(pll position)
{
    // offset and transpose
    return {position.second, fsize - position.first};
}

void countVisibility(vector<Asteroid>& field)
{
    for(int i = 0; i != field.size(); ++i)
    {
        Asteroid& base = field[i];
        pll bposition{transformCoordinates(base.position)};
        set<double> tmp{};
        for(int j = 0; j != field.size(); ++j)
        {
            if (i == j) continue;
            Asteroid candidate = field[j];
            pll cpos{transformCoordinates(candidate.position)};
            pll v = {cpos.first - bposition.first,
                     cpos.second - bposition.second};
            pdd pol = polarCoordinates(v);
            tmp.insert(pol.second);
        }
        base.inRange += tmp.size();
    }
}

Asteroid findBestAsteroid(vector<Asteroid>& field)
{
    Asteroid max{};
    for_each(field.cbegin(), field.cend(),
            [&max](const auto& a){
                if (a.inRange > max.inRange)
                    max = a;
            });
    return max;
}

////////////////////////////////////////////////////////////////////////////////
// main
int main()
{
    int N, M;
    cin >> N >> M;
    fsize = N;
    assert(M == N);

    vector<Asteroid> asteroids{};
    long count{1};
    for(long line = 0; line != N; ++line)
    {
        for(long pos = 0; pos != M; ++pos)
        {
            char val;
            cin >> val;
            cerr << val;
            if (val == '#')
                asteroids.push_back({count++, pll{pos, line}, 0});
        }
        cerr << endl;
    }

    countVisibility(asteroids);

    Asteroid result = findBestAsteroid(asteroids);
    cout << "The best location is " << result.position.first << ", " << result.position.second << endl;
    cout << "The total number of asteroid that can be detected is " << result.inRange << endl;
    //for_each(result.first.cbegin(), result.first.cend(), [](const auto& el){ cout << el << ", "; });
    return 0;
}

