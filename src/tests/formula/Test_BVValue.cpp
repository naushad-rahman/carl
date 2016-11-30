#include "gtest/gtest.h"

#include <carl/formula/bitvector/BVValue.h>

#include <boost/dynamic_bitset.hpp>

#include "../Common.h"

using BDB = boost::dynamic_bitset<>;

template<typename T>
class BVValueTest: public testing::Test {
protected:
	
	carl::BVValue bv;
	carl::BVValue bv32_0;
	carl::BVValue bv32_1;
	carl::BVValue bv32_e30;
	
	BVValueTest():
		bv32_0(32, T(0)),
		bv32_1(32, T(1)),
		bv32_e30(32, T((1 << 30) - 1))
	{}
};

TYPED_TEST_CASE(BVValueTest, IntegerTypes);


TYPED_TEST(BVValueTest, Constructor)
{
	EXPECT_EQ(BDB(32, 0), BDB(this->bv32_0));
	EXPECT_EQ(BDB(32, 1), BDB(this->bv32_1));
	EXPECT_EQ(BDB(32, 1073741823), BDB(this->bv32_e30));
}

TYPED_TEST(BVValueTest, width)
{
	EXPECT_EQ(32, this->bv32_0.width());
	EXPECT_EQ(32, this->bv32_1.width());
	EXPECT_EQ(32, this->bv32_e30.width());	
}

TYPED_TEST(BVValueTest, toString)
{
	EXPECT_EQ("#b00000000000000000000000000000000", this->bv32_0.toString());
	EXPECT_EQ("#b00000000000000000000000000000001", this->bv32_1.toString());
	EXPECT_EQ("#b00111111111111111111111111111111", this->bv32_e30.toString());
}

TYPED_TEST(BVValueTest, isZero)
{
	EXPECT_TRUE(this->bv32_0.isZero());
	EXPECT_FALSE(this->bv32_1.isZero());
	EXPECT_FALSE(this->bv32_e30.isZero());
}

TYPED_TEST(BVValueTest, UnaryMinus)
{
	EXPECT_EQ(carl::BVValue(32, 0), -this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, -1), -this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, -1073741823), -this->bv32_e30);
}

TYPED_TEST(BVValueTest, BinaryInversion)
{
	EXPECT_EQ(carl::BVValue(32, 4294967295), ~this->bv32_0);
	EXPECT_EQ(carl::BVValue(32, 4294967294), ~this->bv32_1);
	EXPECT_EQ(carl::BVValue(32, 3221225472), ~this->bv32_e30);
}

TYPED_TEST(BVValueTest, rotateLeft)
{
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0.rotateLeft(1));
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0.rotateLeft(5));
	EXPECT_EQ(carl::BVValue(32, 2), this->bv32_1.rotateLeft(1));
	EXPECT_EQ(carl::BVValue(32, 32), this->bv32_1.rotateLeft(5));
	EXPECT_EQ(carl::BVValue(32, 2147483646), this->bv32_e30.rotateLeft(1));
	EXPECT_EQ(carl::BVValue(32, 4294967271), this->bv32_e30.rotateLeft(5));
}

TYPED_TEST(BVValueTest, rotateRight)
{
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0.rotateRight(1));
	EXPECT_EQ(carl::BVValue(32, 0), this->bv32_0.rotateRight(5));
	EXPECT_EQ(carl::BVValue(32, 2147483648), this->bv32_1.rotateRight(1));
	EXPECT_EQ(carl::BVValue(32, 134217728), this->bv32_1.rotateRight(5));
	EXPECT_EQ(carl::BVValue(32, 2684354559), this->bv32_e30.rotateRight(1));
	EXPECT_EQ(carl::BVValue(32, 4194303999), this->bv32_e30.rotateRight(5));
}
