#pragma once
#ifndef ZEST_LIB_FUNCTION_H
#define ZEST_LIB_FUNCTION_H

#include <type_traits>
#include <memory>

namespace Zest { namespace Lib {

using in_place_t = struct {};// placeholder for void

template <typename TResult, typename... TArgs>
struct IInvoker
{
	virtual TResult Invoke(TArgs... args) noexcept = 0;
};

template <typename TResult, typename TFunc, typename... TArgs>
class Invoker final : public IInvoker<TResult, TArgs...>
{
public:
	Invoker(TFunc&& func) noexcept(noexcept(std::is_move_constructible_v<TFunc>))
		:m_func(std::move(func))
	{
	}
	TResult Invoke(TArgs... args) noexcept override
	{
		return m_func(std::forward<TArgs>(args)...);
	}
private:
	TFunc m_func;
};

template<typename TResult, typename... TArgs>
class NotWorkInvoker final : public IInvoker<TResult, TArgs...>
{
	TResult Invoke(TArgs... /*args*/) noexcept override
	{
		return TResult();
	}
};

template <typename TDummy> class Function;

template <typename TResult, typename... TArgs>
class Function<TResult(TArgs...)>
{
public:
	template<typename TFunc>
	Function(TFunc&& func) noexcept
		: m_invoker{std::move(MakeInvoker<TFunc>(std::move(func)))}
	{
	}

	Function(in_place_t) noexcept
		: m_invoker(std::move(std::make_unique<NotWorkInvoker<TResult, TArgs...>>()))
	{
	}

	TResult operator()(TArgs... args) noexcept
	{
		return m_invoker->Invoke(std::forward<TArgs>(args)...);
	}

private:
	using InvokerType = IInvoker<TResult, TArgs...>;

	template<typename TFunc>
	static std::unique_ptr<InvokerType> MakeInvoker(TFunc&& func) noexcept
	{
		return std::make_unique<Invoker<TResult, TFunc, TArgs...>>(std::move(func));
	}

	std::unique_ptr<InvokerType> m_invoker;
};

}}

#endif
