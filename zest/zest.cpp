// zest.cpp : Server entry point.
//

#include <iostream>
#include <gtest/gtest.h>

#include "ThreadPool.h"
#include "Error.h"
#include "Maybe.h"
#include "Optional.h"

using namespace Zest::Lib;

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();

	system("PAUSE");
	return 0;
}

