
#include "gtest/gtest.h"
#include "carl/core/Variable.h"

using namespace carl;

TEST(Variable, Constructor)
{
    
}

TEST(Variable, Equals)
{
    Variable v1(1,VariableType::VT_INT);
    Variable v2(2,VariableType::VT_REAL);
    EXPECT_EQ(VariableType::VT_INT,v1.getType());
    EXPECT_EQ((unsigned)1,v1.getId());
    EXPECT_EQ(VariableType::VT_REAL,v2.getType());
    EXPECT_TRUE(v1 != v2);
    EXPECT_TRUE(v1 == v1);
}

TEST(Variable, Comparison)
{
    Variable v1(123);
    Variable v2(456);

    EXPECT_TRUE(v1 < v2);
    EXPECT_FALSE(v2 < v1);
    EXPECT_FALSE(v1 < v1);
    EXPECT_FALSE(v2 < v2);

    EXPECT_FALSE(v1 > v2);
    EXPECT_TRUE(v2 > v1);
    EXPECT_FALSE(v1 > v1);
    EXPECT_FALSE(v2 > v2);


    EXPECT_TRUE(v1 <= v2);
    EXPECT_FALSE(v2 <= v1);
    EXPECT_TRUE(v1 <= v1);
    EXPECT_TRUE(v2 <= v2);

    EXPECT_FALSE(v1 >= v2);
    EXPECT_TRUE(v2 >= v1);
    EXPECT_TRUE(v1 >= v1);
    EXPECT_TRUE(v2 >= v2);
}
