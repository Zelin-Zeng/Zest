#pragma once
#ifndef ZEST_LIB_EXECUTOR_H
#define ZEST_LIB_EXECUTOR_H

#include <type_traits>
#include <functional>

namespace Zest { namespace Lib {

using TFunc = std::function<void()>;

class IExecutor
{
public:
	virtual void Post(TFunc&& func) noexcept = 0;
	virtual ~IExecutor() {}
};

class InlineExecutor : public IExecutor
{
public:
	static InlineExecutor& GetInstance() noexcept
	{
		static InlineExecutor singletonInlineExecutor;
		return singletonInlineExecutor;
	}

	void Post(TFunc&& func) noexcept override
	{
		func();
	}
};

}}
#endif
