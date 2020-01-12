#include <iostream>
#include <vector>
//#include <array>
#include <valarray>
#include <map>
#include <set>
#include <list>
//#include <list>
//#include <memory>
#include <cmath>
#include <numeric>
//#include <sstream>
#include <optional>
#include <chrono>
#include <thread>
#include <bitset>
#include <queue>

using namespace std;
using bigint = __int128;
using coord = std::pair<bigint, bigint>;

std::ostream& operator<<(std::ostream& os, const bigint& obj)
{
    os << static_cast<long long>(obj);
    return os;
}

bigint mod(bigint a, bigint m)
{ return a < 0 ? (a + m) % m : a % m; }

// Modular exponentiation
bigint modpow(bigint x, bigint e, bigint m)
{
    if (e == 0) {
        return (x == 0) ? 0 : 1;
    }
    x = mod(x, m);
    bigint res{1};
    while (e > 0)
    {
        // If e is odd, multimle x with result
        if (e % 2 != 0)
            res = mod(res * x, m);
        // e is even now
        e /= 2;
        x = mod(x * x, m);
    }
    return res;
}

// extended Euclidean Algorithm
tuple<bigint, bigint, bigint> gcdExtended(bigint a, bigint m)
{
    // search ax+my=gcd(a,m)
    // base of recursion
    if (a == 0)
        return {m, 0, 1};

    auto[gcd, x, y] = gcdExtended(m % a, a);
    return {gcd, y - (m / a) * x, x};
}

bigint modInverse(bigint a, bigint m)
{
    // ax + my = g <=> a*y = 1 mod (m)
    auto[g, x, y] = gcdExtended(a, m);
    bigint x1, y1;
    assert(g == 1);
    return mod(x, m);
}

bigint modDivide(bigint a, bigint b, bigint m)
{
    a = mod(a, m);
    bigint inv = modInverse(b, m);
    return (a * inv) % m;
}

struct DeckShuffler
{
    bigint deckSize{10};
    // store the equation: newIdx = a*oldIdx + b
    bigint a{1};
    bigint b{0};

    // every new operation: newIdx = a2 * (a*oldIdx + b) + b2 <=> a2*a*oldIdx + a2*b + b2
    // then % deckSize is taken
    void applyNewFunc(bigint a2, bigint b2)
    {
        a = mod(a * a2, deckSize);
        b = mod(a2 * b + b2, deckSize);
    }

    bigint evalFunc(bigint index)
    { return mod(mod(a*index, deckSize) + b, deckSize); }

    bigint evalTimesAndReverse(bigint times, bigint finalIdx)
    {
        // a*a*oldIdx + a*b + b per iteration
        const bigint pwrA = modpow(a, times, deckSize);
        const bigint newA{pwrA};
        assert(newA < deckSize);
        // Fermat's little theorem to calculate modInverse of a - 1
        const bigint inv{modpow(a - 1, deckSize - 2, deckSize)};
        assert(inv < deckSize);
        // geometric progression b * (a^times - 1) / (a - 1)
        bigint newB{pwrA - 1};
        newB = modDivide(newB, a - 1, deckSize);
        newB *= b;
        newB = mod(newB, deckSize);
        bigint originalIdx{mod(finalIdx - newB, deckSize)};
        originalIdx = modDivide(originalIdx, newA, deckSize);
        return mod(originalIdx, deckSize);
    }

    bigint deal(bigint index)
    {
        const bigint newIdx{deckSize - index - 1};
        applyNewFunc(-1, deckSize - 1);
        return newIdx;
    }

    bigint cut(bigint n, bigint index)
    {
        applyNewFunc(1, -n);
        index -= n;
        if (index >= deckSize)
            index = mod(index, deckSize);
        while(index < 0)
            index += deckSize;
        return index;
    }

    bigint deal(bigint n, bigint index)
    {
        assert(n != 0);
        applyNewFunc(n, 0);
        return mod(n * index, deckSize);
    }

    bigint traverseInstructions(vector<string> const& inputs, bigint target)
    {
        for(auto const& input : inputs)
            target = parseInput(input, target);
        return target;
    }

    bigint parseInput(string const& input, bigint target)
    {
        auto keyword = input.substr(0, 4);
        if (keyword == "deal")
        {
            if (auto second = input.substr(5, 4); second == "with") {
                auto pos = input.rfind(' ');
                auto num = input.substr(pos, input.size() - pos);
                const bigint n = stol(num);
                target = deal(n, target);
            } else {
                target = deal(target);
            }
        }
        else if (keyword == "cut ")
        {
            auto num = input.substr(input.rfind(' '));
            const bigint n = stol(num);
            target = cut(n, target);
        }
        else
            cerr << "shuffle overflow" << endl;
        return target;
    }


    // for tests
    static vector<bigint> deal(vector<bigint>& deck)
    {
        return {deck.rbegin(), deck.rend()};
    }

    static vector<bigint> cut(bigint n, vector<bigint> deck)
    {
        if (n > 0)
            rotate(deck.begin(), deck.begin() + n, deck.end());
        else
            rotate(deck.rbegin(), deck.rbegin() - n, deck.rend());
        return std::move(deck);
    }

    static vector<bigint> deal(bigint n, vector<bigint>& deck)
    {
        bigint newPos = 0;
        vector<bigint> ndeck(deck.size());
        for (bigint i = 0; i != deck.size(); ++i, newPos += n)
            ndeck[mod(newPos, deck.size())] = deck[i];
        return std::move(ndeck);
    }

};

////////////////////////////////////////////////////////////////////////////////
// main
int main()
{
    int N;
    cin >> N;
    string garbage;
    std::getline(cin, garbage);
    vector<string> inputs{};
    inputs.reserve(N);
    for(int i = 0; i < N; ++i)
    {
        string line;
        std::getline(cin, line);
        inputs.push_back(line);
    }

    {
        DeckShuffler testShuffler{10};
        vector<bigint> testDeck{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        // deal
        vector<bigint> res{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
        assert(res == DeckShuffler::deal(testDeck));
        bigint resIdx = 0;
        assert(resIdx == testShuffler.deal(res[resIdx]));
        resIdx = 3;
        assert(resIdx == testShuffler.deal(res[resIdx]));
        for(resIdx = 0; resIdx != res.size(); ++resIdx)
            assert(resIdx == testShuffler.deal(res[resIdx]));
        // cut
        res = {3, 4, 5, 6, 7, 8, 9, 0, 1, 2};
        assert(res == DeckShuffler::cut(3, testDeck));
        resIdx = 0;
        assert(resIdx == testShuffler.cut(3, res[resIdx]));
        resIdx = 4;
        assert(resIdx == testShuffler.cut(3, res[resIdx]));
        for(resIdx = 0; resIdx != res.size(); ++resIdx)
            assert(resIdx == testShuffler.cut(3, res[resIdx]));
        res = {6, 7, 8, 9, 0, 1, 2, 3, 4, 5};
        assert(res == DeckShuffler::cut(-4, testDeck));
        for(resIdx = 0; resIdx != res.size(); ++resIdx)
            assert(resIdx == testShuffler.cut(-4, res[resIdx]));
        // deal with increment
        res = {0, 7, 4, 1, 8, 5, 2, 9, 6, 3};
        assert(res == DeckShuffler::deal(3, testDeck));
        for(resIdx = 0; resIdx != res.size(); ++resIdx)
            assert(resIdx == testShuffler.deal(3, res[resIdx]));
    }

    cout << "Part 1\n";
    {
        DeckShuffler part1{10007};
        const bigint initialIdx = 2019;
        const bigint answ = part1.traverseInstructions(inputs, initialIdx);
        cout << "The position of card " << initialIdx << " is " << answ << endl;
    }

    cout << "Part 2\n";
    {
        DeckShuffler part2{119315717514047};
        part2.traverseInstructions(inputs, 0);

        const bigint times{101741582076661};
        const bigint finalIdx = 2020;
        const bigint answ = part2.evalTimesAndReverse(times, finalIdx);
        cout << "The original position of card " << finalIdx << " is " << answ << endl;
        assert(answ < 72238684032294);
        assert(answ > 35289);
        assert(answ != 72238684017424);
        assert(answ != 72238684032293);
        assert(answ != 95709592746680);
        assert(answ != 45542206702723);
    }

    return 0;
}

