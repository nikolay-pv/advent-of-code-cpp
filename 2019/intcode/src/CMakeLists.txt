cmake_minimum_required(VERSION 3.15)

add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME} PRIVATE main.cpp intcode.hpp intcode.cpp)

# for tests
add_library(${CMAKE_PROJECT_NAME}_lib STATIC main.cpp intcode.hpp intcode.cpp)
