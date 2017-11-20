// zest.cpp : Server entry point.
//

#include <iostream>

#include "ThreadPool.h"
#include "Error.h"
#include "Maybe.h"
#include "Optional.h"
using namespace Zest::Lib;

class A
{

};

int main()
{
	Error::TemplateError e;
	e.GetErrorCode();
	std::string s{ e.GetErrorName() };
	std::cout << s << std::endl;

	Maybe<int> m(1);
	m.GetValue();
	A a;

	Maybe<A> m2(std::move(a));
	m2.GetValue();
	auto pr = std::make_unique<Error::TemplateError>();
	Maybe<A> m3(std::move(pr));
	try
	{
		m2 = std::move(m3);
		m2.GetValue();
	}
	catch (std::exception& e)
	{
		std::cout << e.what();
	}


    return 0;
}

