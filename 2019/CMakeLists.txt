cmake_minimum_required(VERSION 3.11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

project(aoc2019)

add_executable(aoc2019)

target_sources(aoc2019 PRIVATE main.cpp)

target_compile_features(aoc2019 PUBLIC cxx_std_17)

