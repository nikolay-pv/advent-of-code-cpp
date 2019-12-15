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
    pdd polarRelative{};
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
            Asteroid& candidate = field[j];
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

pll transformCoordinatesLaser(pll position)
{
    return {position.second, -position.first};
}

constexpr double Pi = 3.1415926536;

void deployLaser(vector<Asteroid>& field, long idPos)
{
    Asteroid& base = field[idPos-1];
    pll bposition{transformCoordinatesLaser(base.position)};

    //map<double, map<double, pll>> tmp{};
    for(int j = 0; j != field.size(); ++j)
    {
        Asteroid& candidate = field[j];
        if (base.id == candidate.id) continue;
        pll cpos{transformCoordinatesLaser(candidate.position)};
        pll v = {cpos.first - bposition.first,
                 cpos.second - bposition.second};
        pdd t = polarCoordinates(v);
        const double angle = abs(t.second - Pi) < 0.00001 ? 0.0 : t.second + Pi;
        t.second = angle;
        candidate.polarRelative = t;
    }
}

pll simulateDistruction(vector<Asteroid>& field, long place)
{
    map<double, std::vector<Asteroid*>> angled;
    for_each(field.begin(), field.end(),
        [&angled](auto& a){
            if (abs(a.polarRelative.first) < 0.00001)
                return;
            auto found = angled.find(a.polarRelative.second);
            if (found == angled.end())
            {
                angled.insert({a.polarRelative.second, {}});
                found = angled.find(a.polarRelative.second);
            }
            found->second.push_back(&a);
        });
    for_each(angled.begin(), angled.end(),
        [](auto angle){
            auto& as = angle.second;
            sort(as.begin(), as.end(),
                [](const auto& a, const auto& b){
                    return a->polarRelative.first < b->polarRelative.first;
                });
            for(int i = 0; i != as.size(); ++i)
            {
                if (i != 0)
                    assert(as[i-1]->polarRelative.first < as[i]->polarRelative.first);
                as[i]->polarRelative.second += i*2.0*Pi;
            }
        });
    sort(field.begin(), field.end(),
        [](const auto& a, const auto& b){
            return a.polarRelative.second < b.polarRelative.second
                   || (a.polarRelative.second == b.polarRelative.second
                       && a.polarRelative.first < b.polarRelative.first);
        });
    long counter = 0;
    for_each(field.cbegin(), field.cend(),
        [&counter](const auto& el){
            cerr << "R: " << counter++ << "\t" << el << " with polar coordinates: " << el.polarRelative.first << ", " << el.polarRelative.second << endl;
        });
    assert(field[1].position.first == 29 && field[1].position.second == 27);
    // also remove the base from counting
    return field[place].position;
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
            cerr.width(5);
            if (pos == 29 && line == 28)
                cerr << 'X';
            else
                cerr << (val == '#' ? to_string(count) : ".");
            if (val == '#')
                asteroids.push_back({count++, pll{pos, line}, pdd{}, 0});
        }
        cerr << endl;
    }
    cerr.width(0);

    countVisibility(asteroids);

    Asteroid result = findBestAsteroid(asteroids);
    cout << "The best location is " << result.position.first << ", " << result.position.second << endl;
    cout << "The total number of asteroid that can be detected is " << result.inRange << endl;

    const long place = 200;
    const long id = result.id;
    deployLaser(asteroids, id);
    auto answer = simulateDistruction(asteroids, place);
    cout << "The " << place << " asteroid to be vaporized is at " << answer.first << ", " << answer.second << endl;
    cout << "The required math X*100+Y = " << answer.first*100 + answer.second << endl;
    return 0;
}

