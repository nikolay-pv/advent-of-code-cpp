#include "gtest/gtest.h"
#include "intcode.hpp"

TEST(checkTest, test1) {
    //arrange
    //act
    //assert
    EXPECT_EQ(Check::call(0),  0);
    EXPECT_EQ(Check::call(10), 20);
    EXPECT_EQ(Check::call(50), 100);
}
