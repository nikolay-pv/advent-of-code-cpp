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
using coord = std::pair<long, long>;

struct DeckShuffler
{
    long deckSize{10};

    long deal(long index)
    {
        //cerr << "Dealing index " << index << endl;
        return deckSize - index - 1;
    }

    long reverseDeal(long index)
    {
        return deckSize - index - 1;
    }

    long cut(long n, long index)
    {
        //cerr << "Cutting index " << index << endl;
        index -= n;
        if (index >= deckSize)
            index %= deckSize;
        while(index < 0)
            index += deckSize;
        return index;
    }

    long reverseCut(long n, long index)
    {
        index += n;
        if (n > 0)
            index %= deckSize;
        else if (index < 0)
            index = deckSize + index;
        return index;
    }

    long deal(long n, long index)
    {
        //cerr << "Dealing index " << index << " with n " << n << endl;
        return (n * index) % deckSize;
    }

    long reverseDeal(long n, long index)
    {
        if (index % n == 0)
            return index / n;
        const long circle = n - index % n;
        const long offsettedPosition = circle*deckSize + index;
        const long answ = offsettedPosition / n;
        //cerr << "circle " << circle << " offsettedPosition " << offsettedPosition << " answ " << answ << endl;
        return answ;
    }

    // for tests
    static vector<long> deal(vector<long>& deck)
    {
        return {deck.rbegin(), deck.rend()};
    }

    static vector<long> cut(long n, vector<long> deck)
    {
        if (n > 0)
            rotate(deck.begin(), deck.begin() + n, deck.end());
        else
            rotate(deck.rbegin(), deck.rbegin() - n, deck.rend());
        return std::move(deck);
    }

    static vector<long> deal(long n, vector<long>& deck)
    {
        long newPos = 0;
        vector<long> ndeck(deck.size());
        for (int i = 0; i != deck.size(); ++i, newPos += n)
            ndeck[newPos % deck.size()] = deck[i];
        return std::move(ndeck);
    }

    long traverseInstructions(vector<string> const& inputs, long target)
    {
        for(auto const& input : inputs)
            target = parseInput(input, target);
        return target;
    }

    long parseInput(string const& input, long target)
    {
        //cerr << "Input " << input << endl;
        auto keyword = input.substr(0, 4);
        if (keyword == "deal")
        {
            if (auto second = input.substr(5, 4); second == "with") {
                auto pos = input.rfind(' ');
                auto num = input.substr(pos, input.size() - pos);
                const long n = stol(num);
                //cerr << "Extracted " << n << " from " << input << endl;
                target = deal(n, target);
                //cerr << " result = " << target << endl;
            } else {
                //cerr << "Extracted from " << input << endl;
                target = deal(target);
                //cerr << " result = " << target << endl;
            }
        }
        else if (keyword == "cut ")
        {
            auto num = input.substr(input.rfind(' '));
            const long n = stol(num);
            //cerr << "Extracted " << n << " from " << input << endl;
            target = cut(n, target);
            //cerr << " result = " << target << endl;
        }
        else
            cerr << "shuffle overflow" << endl;
        return target;
    }

    long reverseTraverseInstructions(vector<string> const& inputs, long target)
    {
        for(auto const& input : inputs)
            target = reverseInput(input, target);
        return target;
    }

    long reverseInput(string const& input, long target)
    {
        //deal into new stack
        //cut -2
        //deal with increment 7
        //cut 8
        //cut -4
        //deal with increment 7
        //cut 3
        //deal with increment 9
        //deal with increment 3
        auto keyword = input.substr(0, 4);
        if (keyword == "deal")
        {
            if (auto second = input.substr(4, 4); second == "with") {
                auto num = input.substr(input.rfind(' '));
                const long n = stol(num);
                //cerr << "Extracted " << n << " from " << input << endl;
                target = reverseDeal(n, target);
            } else {
                target = reverseDeal(target);
            }
        }
        else if (keyword == "cut ")
        {
            auto num = input.substr(input.rfind(' '));
            const long n = stol(num);
            //cerr << "Extracted " << n << " from " << input << endl;
            target = reverseCut(n, target);
        }
        else
            cerr << "shuffle overflow" << endl;
        return target;
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
    vector<long> testDeck{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    // deal
    vector<long> res{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    assert(res == DeckShuffler::deal(testDeck));
    long resIdx = 0;
    assert(res[resIdx] == testDeck[testShuffler.reverseDeal(resIdx)]);
    assert(resIdx == testShuffler.deal(res[resIdx]));
    resIdx = 3;
    assert(res[resIdx] == testDeck[testShuffler.reverseDeal(resIdx)]);
    assert(resIdx == testShuffler.deal(res[resIdx]));
    for(resIdx = 0; resIdx != res.size(); ++resIdx)
    {
        //cerr << "checking " << resIdx << endl;
        assert(res[resIdx] == testDeck[testShuffler.reverseDeal(resIdx)]);
        assert(resIdx == testShuffler.deal(res[resIdx]));
    }
    // cut
    res = {3, 4, 5, 6, 7, 8, 9, 0, 1, 2};
    assert(res == DeckShuffler::cut(3, testDeck));
    resIdx = 0;
    assert(res[resIdx] == testDeck[testShuffler.reverseCut(3, resIdx)]);
    assert(resIdx == testShuffler.cut(3, res[resIdx]));
    resIdx = 4;
    assert(res[resIdx] == testDeck[testShuffler.reverseCut(3, resIdx)]);
    assert(resIdx == testShuffler.cut(3, res[resIdx]));
    for(resIdx = 0; resIdx != res.size(); ++resIdx)
    {
        //cerr << "checking " << resIdx << endl;
        assert(res[resIdx] == testDeck[testShuffler.reverseCut(3, resIdx)]);
        assert(resIdx == testShuffler.cut(3, res[resIdx]));
    }
    res = {6, 7, 8, 9, 0, 1, 2, 3, 4, 5};
    assert(res == DeckShuffler::cut(-4, testDeck));
    for(resIdx = 0; resIdx != res.size(); ++resIdx)
    {
        cerr << "checking " << resIdx << endl;
        assert(res[resIdx] == testDeck[testShuffler.reverseCut(-4, resIdx)]);
        assert(resIdx == testShuffler.cut(-4, res[resIdx]));
    }
    // deal with increment
    res = {0, 7, 4, 1, 8, 5, 2, 9, 6, 3};
    assert(res == DeckShuffler::deal(3, testDeck));
    for(resIdx = 0; resIdx != res.size(); ++resIdx)
    {
        //cerr << "checking " << resIdx << endl;
        assert(res[resIdx] == testDeck[testShuffler.reverseDeal(3, resIdx)]);
        assert(resIdx == testShuffler.deal(3, res[resIdx]));
    }
    }

    // part 1
    cout << "Part 1\n";
    DeckShuffler part1{10007};
    const long initialIdx = 2019;
    const long answ = part1.traverseInstructions(inputs, initialIdx);
    cout << "the position of card " << initialIdx << " is " << answ << endl;
    // part 2
    cout << "Part 2\n";

    return 0;
}

