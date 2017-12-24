#include "CommonTest.h"

#include <gtest/gtest.h>

#include "Dynamics.h"

namespace Zest { namespace Lib {

TEST(DynamicsTest, CreateDynamicArray_GetObjectByIndex)
{
	Dynamic dynamic1({ "one", "two" });
	EXPECT_EQ(dynamic1.GetType(), ValueMap::Type::Array);
	Dynamic dynamic2{ std::move(dynamic1[0]) };
	EXPECT_EQ(dynamic2.GetType(), ValueMap::Type::String);

	Dynamic dynamic3{ std::move(dynamic1[2]) };
	EXPECT_EQ(dynamic3.GetType(), ValueMap::Type::Null);
}

}}

