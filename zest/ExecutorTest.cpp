#include "CommonTest.h"

#include <gtest/gtest.h>

#include "Executor.h"

namespace Zest { namespace Lib {

TEST(ExecutorTest, InlineExecutor_Post_Function_ShouldBeCalled)
{
	InlineExecutor& inlineExecutor{ InlineExecutor::GetInstance() };

	bool isCalled{ false };
	inlineExecutor.Post([&isCalled]() {
		isCalled = true;
	});

	EXPECT_TRUE(isCalled);
}

}}

