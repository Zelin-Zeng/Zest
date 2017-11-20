// zest.cpp : Server entry point.
//

#include <iostream>

#include "ThreadPool.h"
#include "Error.h"
#include "Maybe.h"
#include "Optional.h"
using namespace Zest::Lib;

struct Value
{
	const int X = 1;
	int Y = 2;
};

std::future<int> foo(std::thread& thread) {
	auto promise = std::make_shared<std::promise<int>>();

	thread = std::thread([promise]()
	{
		promise->set_value(1);
	});

	return promise->get_future();
}

int main()
{
	Details::OptionalStorage<int, std::is_trivially_destructible_v<int>> os;
	os.ConstructInPlace(1);
	int* value = os.GetValuePointer();

	if (value)
	{
		std::cout << "value: " << *value << std::endl;
	}

	Details::OptionalStorage<Value, std::is_trivially_destructible_v<Value>> os2;
	os2.ConstructInPlace();
	struct Value* vp = os2.GetValuePointer();
	if (vp)
	{
		std::cout << "value2:" << vp->X << std::endl;
	}

	Optional<struct Value> vo{};
	if (vo == nullopt)
	{
		std::cout << "null" << std::endl;
	}

	system("PAUSE");
    return 0;
}

