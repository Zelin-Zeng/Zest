#pragma once
#ifndef ZEST_LIB_OPTIONAL_H
#define ZEST_LIB_OPTIONAL_H

#include <type_traits>

#include "Error.h"

namespace Zest { namespace Lib {

// Placeholder
struct in_place_t
{
};

struct TNull
{
};

const TNull nullopt{};

namespace Details {

template<typename TValue, bool IsTriviallyDestructible>
struct OptionalStorage
{
	bool Engaged() const noexcept
	{
		return m_engaged;
	}

	TValue& Storage() noexcept
	{
		return const_cast<TValue&>(const_cast<const OptionalStorage*>(this)->Storage());
	}

	const TValue& Storage() const noexcept
	{
		return *reinterpret_cast<const TValue*>(&m_storage);
	}

	void* RawStorage() noexcept
	{
		return &m_storage;
	}

	template<typename... TArgs>
	void ConstructInPlace(TArgs&&... args) noexcept(std::is_nothrow_constructible_v<TValue, TArgs...>)
	{
		if (Engaged())
		{
			Error::ThrowUnexpectOperationErrorException();
		}

		// Placement new
		new(this->RawStorage()) TValue(std::forward<TArgs>(args)...);
		m_engaged = true;
	}

	bool m_engaged = false;

	typename std::aligned_storage_t<sizeof(TValue), std::alignment_of_v<TValue>>::type m_storage;
};

template<typename TValue>
struct OptionalStorage<TValue, false>
{
	~OptionalStorage() noexcept
	{
		if (Engaged())
		{
			Storage().~TValue();
			n_engaged = false;
		}
	}
};

}
}}
#endif