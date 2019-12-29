cmake_minimum_required(VERSION 3.15)

add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME} PRIVATE main.cpp intcode.hpp intcode.cpp
    hull_painting_robot.hpp hull_painting_robot.cpp
    arcade_cabinet.hpp arcade_cabinet.cpp
    repair_droid.hpp repair_droid.cpp
    ascii.hpp ascii.cpp
    tractor_beam.hpp tractor_beam.cpp
    )

# for tests
add_library(${CMAKE_PROJECT_NAME}_lib STATIC main.cpp intcode.hpp intcode.cpp
    hull_painting_robot.hpp hull_painting_robot.cpp
    arcade_cabinet.hpp arcade_cabinet.cpp
    repair_droid.hpp repair_droid.cpp
    ascii.hpp ascii.cpp
    tractor_beam.hpp tractor_beam.cpp
    )
