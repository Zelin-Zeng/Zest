#pragma once
#ifndef ZEST_LIB_DYNAMIC_H
#define ZEST_LIB_DYNAMIC_H

#include <cstdint>
#include <string>
#include <vector>
#include <type_traits>

#include "Error.h"

namespace Zest { namespace Lib {

class Dynamic;

struct ValueMap
{
	using Null = struct {};
	using Object = Dynamic;
	using Array = std::vector<Dynamic>;
	using Bool = bool;
	using Int32 = int32_t;
	using Int64 = int64_t;
	using Double = double;
	using String = std::string;

	enum class Type :uint32_t
	{
		Null,
		Object,
		Array,
		Bool,
		Int32,
		Int64,
		Double,
		String
	};

};

class IDynamicData
{
public:
	enum class CompareResult : uint32_t
	{
		NotComparable,
		Less,
		Larger
	};

	virtual CompareResult Compare(const Dynamic& other) noexcept = 0;

protected:
	virtual ValueMap::Array GetArray()
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual ValueMap::Object GetObject()
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual ValueMap::Int32 GetInt32()
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual ValueMap::Int64 GetInt64()
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual ValueMap::Double GetDouble()
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual ValueMap::String GetString()
	{
		Error::ThrowAcessDeniedErrorException();
	}
};

template<enum class ValueMap::Type>
class DynamicData : public IDynamicData
{

};


class Dynamic
{
public:

	ValueMap::Type GetType() const noexcept
	{
		return m_type;
	}



private:
	ValueMap::Type m_type;

};

}}
#endif
