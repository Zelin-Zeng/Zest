#include "CommonTest.h"

#include <gtest/gtest.h>

#include "Function.h"

namespace Zest { namespace Lib {

TEST(FunctionTest, NormalFunction_Invoke_Lambda_ShouldBeCalled)
{
	bool isCalled{ false };
	Function<void(int)> function { [&isCalled](int i)
	{
		isCalled = true;
		EXPECT_EQ(i, 1);
	}};

	function(1);

	EXPECT_TRUE(isCalled);
}

TEST(FunctionTest, EmptyFunction_Invoke_Empty_ReturnDefaultValue)
{
	Function<int(int)> function({});

	EXPECT_EQ(function(1), 0);
}

TEST(FunctionTest, VoidFunction_Invoke_Lambda_ShouldBeCalled)
{
	bool isCalled{ false };
	Function<void(int)> function([&isCalled](int i)
	{
		isCalled = true;
		EXPECT_EQ(i, 1);
	});

	function(1);
	EXPECT_TRUE(isCalled);

	isCalled = false;
	Function<void(void)> function2([&isCalled]()
	{
		isCalled = true;
	});
	function2();
	EXPECT_TRUE(isCalled);
}

}}

