cmake_minimum_required(VERSION 3.15)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

project(intcode)
set(CMAKE_CXX_STANDARD 17)
#target_compile_features(intcode PUBLIC cxx_std_17)

include_directories(src)

add_subdirectory(src)
add_subdirectory(tst)
add_subdirectory(../../lib/googletest lib/googletest)

