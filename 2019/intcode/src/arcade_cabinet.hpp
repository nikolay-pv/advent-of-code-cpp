#pragma once
#include "intcode.hpp"

enum Tile
{
    Empty = 0,
    Wall = 1,
    Block = 2,
    HorizontalPaddle = 3,
    Ball = 4
};

char getGlyph(Tile tile);

class ArcadeCabinet
{
    IntCodeComputer& runner;
public:
    ArcadeCabinet() = delete;
    ArcadeCabinet(ArcadeCabinet&&) = default;
    ArcadeCabinet(const ArcadeCabinet&) = default;
    ~ArcadeCabinet() = default;

    ArcadeCabinet(IntCodeComputer&);

private:
    void insertDate(std::vector<long>& data);

public:
    void run(optional<long> coins);
    long getBlocksCount() const;
    void flushOuptut(bool getBack) const;

    pair<long, long> ballPos{};
    pair<long, long> paddlePos{};
private:
    map<pair<long, long>, Tile> theMap{};
    long score = 0;
};

