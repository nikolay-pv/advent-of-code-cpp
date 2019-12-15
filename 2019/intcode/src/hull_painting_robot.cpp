#include "hull_painting_robot.hpp"

//#define ROBOT_DEBUG

PaintingRobot::FacingDirection rotate(PaintingRobot::FacingDirection current, Rotation direction)
{
    PaintingRobot::FacingDirection result;
    switch(current) {
        case PaintingRobot::Left  : result = direction == Left ? PaintingRobot::Down  : PaintingRobot::Up;    break;
        case PaintingRobot::Up    : result = direction == Left ? PaintingRobot::Left  : PaintingRobot::Right; break;
        case PaintingRobot::Right : result = direction == Left ? PaintingRobot::Up    : PaintingRobot::Down;  break;
        case PaintingRobot::Down  : result = direction == Left ? PaintingRobot::Right : PaintingRobot::Left;  break;
    }
    return result;
}

PaintingRobot::PaintingRobot(IntCodeComputer& computer)
    : brain{computer}
{};

PaintingRobot::PaintColor PaintingRobot::getCurrentPlateColor() const
{
    auto f = panelMap.find(position);
    if (f == panelMap.end())
        return PaintColor::Black;
    return f->second;
}

bool PaintingRobot::paintCurrentPlate(PaintingRobot::PaintColor withColor)
{
#ifdef ROBOT_DEBUG
    for_each(panelMap.cbegin(), panelMap.cend(), [](const auto& el){cerr << el.first.first << ", " << el.first.second << "; ";});
    cerr << endl << endl;
#endif
    return panelMap.insert_or_assign(position, withColor).second;
}

void PaintingRobot::updatePosition(Rotation rotation)
{
    direction = rotate(direction, rotation);
#ifdef ROBOT_DEBUG
    cerr << "Rotated to " << char(direction) << endl;
#endif
    pair<long, long> increment{};
    switch(direction)
    {
        case Left  : increment = {-1,  0}; break;
        case Up    : increment = { 0,  1}; break;
        case Right : increment = { 1,  0}; break;
        case Down  : increment = { 0, -1}; break;
    }
    position = {position.first + increment.first, position.second + increment.second};
#ifdef ROBOT_DEBUG
    cerr << "Moved to " << position.first << ", " << position.second << endl;
#endif
}

long PaintingRobot::paint()
{
    IntCodeComputer::State brainState = IntCodeComputer::Running;
    long painted = 0;
    while(brainState != IntCodeComputer::Halt)
    {
        // get current panel color
        const long color = getCurrentPlateColor();
        // pass to the brain by running loop
        // get the color as a result
        long newColor{};
        std::tie(newColor, brainState) = brain.runningLoop(color);
        // paint the panel
        if (paintCurrentPlate(PaintColor{newColor}))
            ++painted;
        // get the new direction
        long rotation{};
        std::tie(rotation, brainState) = brain.runningLoop(nullopt);
        // update the position
        updatePosition(static_cast<Rotation>(rotation));
    }
    return painted;
}
