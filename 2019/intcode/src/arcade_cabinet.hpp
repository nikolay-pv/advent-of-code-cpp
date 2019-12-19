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

public:
    void run();
    long getBlocksCount() const;
    void flushOuptut() const;

public:
    map<pair<long, long>, Tile> theMap{};
};

