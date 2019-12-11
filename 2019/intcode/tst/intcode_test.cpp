#include "gtest/gtest.h"
#include "intcode.hpp"

void simpleMemoryTestHelper(std::string message,
                            std::vector<long> input,
                            std::vector<long> result)
{
    IntCodeComputer computer{message, input};
    setUpInstructions(computer.instructionSet);
    computer.runningLoop(0);
    ASSERT_EQ(computer.memory, result);
}

TEST(IntCodeComputerTest, multiplySumDay2) {
    simpleMemoryTestHelper(
            "Simple multiplication and summation",
            {1,9,10,3,2, 3,11,0,99,30, 40,50},
            {3500,9,10,70,2, 3,11,0,99,30, 40,50});
    simpleMemoryTestHelper(
            "sum",
            {1,0,0,0,99},
            {2,0,0,0,99});
    simpleMemoryTestHelper(
            "multiply",
            {2,3,0,3,99},
            {2,3,0,6,99});
    simpleMemoryTestHelper(
            "more multiply",
            {2,4,4,5,99, 0},
            {2,4,4,5,99, 9801});
    simpleMemoryTestHelper(
            "more sum",
            {1,1,1,4,99, 5,6,0,99},
            {30,1,1,4,2, 5,6,0,99});
}

void simpleOutputTestHelper(std::string message,
                            std::vector<long> input,
                            long trueValue)
{
    {
    IntCodeComputer computer{message, input};
    setUpInstructions(computer.instructionSet);
    auto [res, state] = computer.runningLoop(trueValue);
    ASSERT_EQ(1, res);
    }

    {
    IntCodeComputer computer{message, input};
    setUpInstructions(computer.instructionSet);
    auto [res, state] = computer.runningLoop(trueValue + 1);
    ASSERT_EQ(0, res);
    }
}

TEST(IntCodeComputerTest, ioAndImmediateParamModeDay5) {
    simpleOutputTestHelper(
        "if input is equal to 8; output 1 (if it is) or 0 (if it is not)",
        {3,9,8,9,10, 9,4,9,99,-1, 8},
        8);

    simpleOutputTestHelper(
        "if input is less than 8; output 1 (if it is) or 0 (if it is not)",
        {3,9,7,9,10, 9,4,9,99,-1, 8},
        7);

    simpleOutputTestHelper(
        "(immediate mode) if input is equal to 8; output 1 (if it is) or 0 (if it is not)",
        {3,3,1108,-1,8, 3,4,3,99},
        8);

    simpleOutputTestHelper(
        "(immediate mode) if input is less than 8; output 1 (if it is) or 0 (if it is not)",
        {3,3,1107,-1,8, 3,4,3,99},
        7);

    simpleOutputTestHelper(
        "jump tests that take an input, then output 0 if the input was zero or 1 if the input was non-zero",
        {3,3,1107,-1,8, 3,4,3,99},
        7);
}

TEST(IntCodeComputerTest, jumpTestDay5) {
    simpleOutputTestHelper(
        "jump tests that take an input, then output 0 if the input was zero or 1 if the input was non-zero",
        {3,12,6,12,15,1,13,14,13,4,13,99,-1,0,1,9},
        -1);

    simpleOutputTestHelper(
        "(immediate mode) jump tests that take an input, then output 0 if the input was zero or 1 if the input was non-zero",
        {3,3,1105,-1,9,1101,0,0,12,4,12,99,1},
        -1);

    //The next program uses an input instruction to ask for a single number.
    //The program will then output 999 if the input value is below 8,
    //output 1000 if the input value is equal to 8,
    //or output 1001 if the input value is greater than 8.
    vector<long> input{3,21,1008,21,8, 20,1005,20,22,107, 8,21,20,1006,20,
                       31,1106,0,36,98, 0,0,1002,21,125, 20,4,20,1105,1,
                       46,104,999,1105,1, 46,1101,1000,1,20, 4,20,1105,1,46,
                       98,99};
    {
    IntCodeComputer computer{"Below 8", input};
    setUpInstructions(computer.instructionSet);
    auto [res, state] = computer.runningLoop(7);
    ASSERT_EQ(999, res);
    }

    {
    IntCodeComputer computer{"Equal 8", input};
    setUpInstructions(computer.instructionSet);
    auto [res, state] = computer.runningLoop(8);
    ASSERT_EQ(1000, res);
    }

    {
    IntCodeComputer computer{"Above 8", input};
    setUpInstructions(computer.instructionSet);
    auto [res, state] = computer.runningLoop(9);
    ASSERT_EQ(1001, res);
    }
}
