#include "CommonTest.h"

#include <gtest/gtest.h>

#include "json/json.h"

namespace Zest { namespace Lib {

TEST(JsonTest, NormalTest)
{
	using namespace Json;
	JsonValue<JsonValueType::Boolean> value{ true };
	JsonValue<JsonValueType::Null> value2{};
	EXPECT_TRUE(value.GetBoolean());

	std::string jsonString1{ "testjson stream" };
	JsonStringStream jss1{ jsonString1 };

	std::stringstream ss1;

	while (!jss1.IsEnd())
	{
		ss1 << jss1.Read();
	}

	EXPECT_EQ(ss1.str(), jsonString1);

	const char* jsonString2{ "testjson stream2" };
	JsonStringStream jss2{ jsonString2, strlen(jsonString2) };
	std::stringstream ss2;

	while (!jss2.IsEnd())
	{
		ss2 << jss2.Read();
	}

	EXPECT_EQ(ss2.str(), jsonString2);
}

}}

