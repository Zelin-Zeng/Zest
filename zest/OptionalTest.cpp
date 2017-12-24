#include "CommonTest.h"

#include <gtest/gtest.h>

#include "Optional.h"

namespace Zest { namespace Lib {

TEST(OptionalTest, OptionalInt_EmptyOrNonEmpty)
{
	Optional<int> opt1{ 1 };
	EXPECT_TRUE(opt1 != nullopt);
	EXPECT_EQ(*opt1, 1);

	Optional<int> opt2;
	EXPECT_TRUE(opt2 == nullopt);
}

TEST(OptionalTest, OptionalDouble_EmptyOrNonEmpty)
{
	Optional<double> opt1{ 1.0 };
	EXPECT_TRUE(opt1 != nullopt);
	EXPECT_EQ(*opt1, 1.0);

	Optional<double> opt2;
	EXPECT_TRUE(opt2 == nullopt);
}

TEST(OptionalTest, OptionalString_EmptyOrNonEmpty)
{
	Optional<std::string> opt1{ "abcd" };
	EXPECT_TRUE(opt1 != nullopt);
	EXPECT_EQ(*opt1, "abcd");

	Optional<std::string> opt2;
	EXPECT_TRUE(opt2 == nullopt);
}

TEST(OptionalTest, OptionalSimpleClass_EmptyOrNonEmpty)
{
	class SimpleObj
	{
	public:
		SimpleObj(const char* charValue) noexcept
			: m_string{ charValue ? charValue : std::string() }
		{
		}

		SimpleObj(const std::string stringValue) noexcept
			: m_string{ stringValue }
		{
		}

		const std::string& GetString() const noexcept
		{
			return m_string;
		}
	private:
		std::string m_string;
	};

	Optional<SimpleObj> opt1{ "abcd" };
	EXPECT_TRUE(opt1 != nullopt);
	EXPECT_EQ((*opt1).GetString(), "abcd");

	Optional<SimpleObj> opt2;
	EXPECT_TRUE(opt2 == nullopt);
}

}}

