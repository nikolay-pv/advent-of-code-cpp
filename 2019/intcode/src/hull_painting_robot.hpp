#pragma once
#include "intcode.hpp"

// Positive is contra clockwise
enum Rotation : int
{
    Left = 0,
    Right = 1
};

class PaintingRobot
{
    IntCodeComputer& brain;
public:
    PaintingRobot() = delete;
    PaintingRobot(PaintingRobot&&) = default;
    PaintingRobot(const PaintingRobot&) = default;
    ~PaintingRobot() = default;

    PaintingRobot(IntCodeComputer&);

public:
    enum FacingDirection : char
    {
        Left = '<',
        Up = '^',
        Right = '>',
        Down = 'v'
    };

    enum PaintColor : long
    {
        Black = 0,
        White = 1
    };

    PaintColor getCurrentPlateColor() const;
    // returns true if the first time painting
    bool paintCurrentPlate(PaintColor withColor);
    void updatePosition(Rotation);
    long paint();
    void displayPainted() const;

public:
    FacingDirection direction = Up;
    map<pair<long, long>, PaintColor> panelMap{{{0, 0}, PaintColor::White}};
    pair<long, long> position{0, 0};
};

PaintingRobot::FacingDirection rotate(PaintingRobot::FacingDirection current, Rotation direction);

