#pragma once
#ifndef ZEST_LIB_MAYBE_H
#define ZEST_LIB_MAYBE_H

#include <type_traits>
#include <cstdint>
#include <memory>

#include "Error.h"

namespace Zest { namespace Lib {

template<typename TValue>
class Maybe
{
public:
	static_assert(!std::is_reference<TValue>::value, "Maybe doesn't accept reference type.");

	Maybe(TValue&& value) noexcept(std::is_nothrow_move_constructible<TValue>::value)
		: m_value{ std::move(value) }, m_isValue{ true }, m_isRetrieved{ false }
	{
	}

	Maybe(std::unique_ptr<Error::IError> error) noexcept
		: m_pError{ std::move(error) }, m_isValue{ false }, m_isRetrieved{ false }
	{
	}

	using in_place_t = struct {};// placeholder for void

	template<typename... TArgs>
	Maybe(in_place_t, TArgs&&... args) noexcept(noexcept(::new (nullptr) TValue(std::declval<TArgs&&>()...)))
		: m_value{ std::forward<TArgs>(args)... }, m_isValue{ true }, m_isRetrieved{ false }
	{
	}

	Maybe(Maybe<TValue>&& other) noexcept
		: m_value{ std::move(other.m_value) }, m_isValue{ other.m_isValue }, m_pError{ std::move(other.m_pError) }, m_isRetrieved{ other.m_isRetrieved }
	{
	}

	Maybe& operator=(Maybe<TValue>&& other) noexcept
	{
		if (this != &other)
		{
			std::swap(m_isRetrieved, other.m_isRetrieved);
			std::swap(m_isValue, other.m_isValue);
			if (!m_isValue)
			{
				m_pError = std::move(other.m_pError);
			}
			else
			{
				std::swap(m_value, other.m_value);
			}
		}
		return *this;
	}

	bool IsValue() const
	{
		return m_isValue;
	}

	TValue GetValue()
	{
		if (!m_isValue || m_isRetrieved)
			Error::ThrowAcessDeniedErrorException();
		m_isRetrieved = false;
		return std::move(m_value);
	}

	std::unique_ptr<Error::IError> GetError()
	{
		if (m_isValue || m_isRetrieved)
			Error::ThrowAcessDeniedErrorException();
		m_isRetrieved = false;
		return std::move(m_pError);
	}

private:
	std::unique_ptr<Error::IError> m_pError;
	TValue m_value;
	bool m_isValue;
	bool m_isRetrieved;
};


template<>
class Maybe<void>
{
public:
	Maybe() noexcept
		: m_isValue{ true }, m_isRetrieved{ false }
	{
	}

	Maybe(std::unique_ptr<Error::IError> error) noexcept
		: m_pError{ std::move(error) }, m_isValue{ false }, m_isRetrieved{ false }
	{
	}

	bool IsValue() const
	{
		return m_isValue;
	}

	std::unique_ptr<Error::IError> GetError()
	{
		if (m_isValue || m_isRetrieved)
			Error::ThrowAcessDeniedErrorException();
		m_isRetrieved = false;
		return std::move(m_pError);
	}

private:
	std::unique_ptr<Error::IError> m_pError;
	bool m_isValue;
	bool m_isRetrieved;
};

}}
#endif
