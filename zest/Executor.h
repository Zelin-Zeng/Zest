#pragma once
#ifndef ZEST_LIB_EXECUTOR_H
#define ZEST_LIB_EXECUTOR_H

#include <type_traits>

namespace Zest { namespace Lib {

class IExecutor
{
public:
	 template <class TFunc, class... TArgs>
	 virtual void Post(TFunc&& func, TArgs&&... args) noexcept = 0;
};

class InlineExecutor : public IExecutor
{
	 static InlineExecutor& GetInstance() noexcept
	 {
		  static InlineExecutor singletonInlineExecutor;
		  return singletonInlineExecutor;
	 }

	 template <class TFunc, class... TArgs>
	 void Post(TFunc&& func, TArgs&&... args) noexcept override
	 {
		  func(std::forward<TArgs>(args)...);
	 }
};

}}


#endif
