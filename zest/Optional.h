#pragma once
#ifndef ZEST_LIB_OPTIONAL_H
#define ZEST_LIB_OPTIONAL_H

#include <new>
#include <type_traits>

#include "Error.h"

namespace Zest { namespace Lib {

struct TNull
{
};

const TNull nullopt{};

namespace Details {

template<typename TValue, bool IsTriviallyDestructible>
struct OptionalStorage
{
	bool IsEngaged() const noexcept
	{
		return m_engaged;
	}

	TValue* GetValuePointer() noexcept
	{
		if (IsEngaged())
		{
			// Maybe this doesn't work? But I tried struct with const, and it passed.
			return reinterpret_cast<TValue*>(m_storage);
		}
		return nullptr;
	}

	TValue& GetValue() noexcept
	{
		if (!IsEngaged())
		{
			Error::ThrowAcessDeniedErrorException();
		}

		return *GetRawValuePointer();
	}

	void* GetRawValuePointer() noexcept
	{
		return static_cast<void*>(m_storage);
	}

	template<typename... TArgs>
	void ConstructInPlace(TArgs&&... args) noexcept(std::is_nothrow_constructible_v<TValue, TArgs...>)
	{
		// Optional cannot be initialized twice.
		if (IsEngaged())
		{
			Error::ThrowUnexpectOperationErrorException();
		}

		// Placement new
		new(this->GetRawValuePointer()) TValue(std::forward<TArgs>(args)...);
		m_engaged = true;
	}

	void Clear() noexcept
	{
		if (IsEngaged())
		{
			GetValuePointer()->~TValue();
			m_engaged = false;
		}
	}

	bool m_engaged = false;

	typename std::aligned_storage_t<sizeof(TValue), std::alignment_of_v<TValue>> m_storage[1];
};

template<typename TValue>
struct OptionalStorage<TValue, false> : public OptionalStorage<TValue, true>
{
	~OptionalStorage() noexcept
	{
		this->Clear();
	}
};

}//Details

template<typename TValue>
class Optional : Details::OptionalStorage<TValue, std::is_trivially_destructible_v<TValue>>
{
	template<class TUnknown>
	friend constexpr bool operator==(const Optional<TUnknown>& lhs, TNull) noexcept;
	template<class TUnknown>
	friend constexpr bool operator!=(const Optional<TUnknown>& lhs, TNull) noexcept;
public:
	static_assert(!std::is_reference<TValue>::value, "Optional may not be used with reference types");
	static_assert(!std::is_abstract<TValue>::value, "Optional may not be used with abstract types");

	constexpr Optional() noexcept
	{
	}

	Optional(Optional&& other) noexcept(std::is_nothrow_copy_constructible_v<TValue>)
	{
		if (other.IsEngaged())
		{
			ConstructInPlace(std::move(*other.GetValuePointer()));
			other.Clear();
		}
	}

	template<typename... TArgs>
	Optional(TNull, TArgs&&... args) noexcept(std::is_nothrow_constructible_v<TValue, TArgs...>)
	{
		ConstructInPlace(std::forward<TArgs>(args)...);
	}

	Optional(TValue&& value) noexcept(std::is_nothrow_move_constructible_v<TValue>)
	{
		ConstructInPlace(std::move(value));
	}

	Optional(const TValue& value) noexcept(std::is_nothrow_copy_constructible_v<TValue>)
	{
		if (other.IsEngaged())
		{
			ConstructInPlace(value);
		}
	}

	Optional(TNull) noexcept
	{
	}

	Optional& operator=(TNull) noexcept
	{
		this->Clear();
		return *this;
	}

	TValue& operator*() noexcept
	{
		return *(this->GetValuePointer());
	}

	bool operator()() noexcept
	{
		return this->IsEngaged();
	}
};

template<typename TValue>
constexpr bool operator==(const Optional<TValue>& lhs, TNull) noexcept
{
	return !lhs.IsEngaged();
}

template<typename TValue>
constexpr bool operator==(const Optional<TValue>& lhs, const Optional<TValue>& rhs) noexcept
{
	if ((lhs == nullopt) ^ (lhs == nullopt))
	{
		return false;
	}

	if (lhs == nullopt)
	{
		return true;
	}

	return lhs.GetValue() == rhs.GetValue();
}

template<typename TValue>
constexpr bool operator!=(const Optional<TValue>& lhs, const Optional<TValue>& rhs) noexcept
{
	return !(lhs == rhs);
}

template<typename TValue>
constexpr bool operator!=(const Optional<TValue>& lhs, TNull) noexcept
{
	return lhs.IsEngaged();
}

}}

#endif
