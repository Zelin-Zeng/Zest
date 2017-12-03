#pragma once
#ifndef ZEST_LIB_DYNAMIC_H
#define ZEST_LIB_DYNAMIC_H

#include <cstdint>
#include <string>
#include <vector>
#include <type_traits>
#include <unordered_map>
#include <limits>

#include "Error.h"

namespace Zest { namespace Lib {

class IDynamicData;
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

class Dynamic
{
public:
	Dynamic();
	Dynamic(Dynamic&& dynamicValue);
	Dynamic(const Dynamic& dynamicValue);
	Dynamic(ValueMap::Array&& arrayValue);
	Dynamic(std::initializer_list<ValueMap::Object> initlist);
	Dynamic(std::initializer_list<std::pair<ValueMap::String, ValueMap::Object>> initlist);
	Dynamic(const ValueMap::String& stringValue);
	Dynamic(const char* charValue);
	Dynamic(const ValueMap::Bool boolValue);
	Dynamic(const ValueMap::Int32 int32Value);
	Dynamic(const ValueMap::Int64 int64Value);
	Dynamic(const ValueMap::Double doubleValue);

	ValueMap::Array& GetArray();
	ValueMap::Bool GetBool() const;
	ValueMap::Int32 GetInt32() const;
	ValueMap::Int64 GetInt64() const;
	ValueMap::Double GetDouble() const;
	const ValueMap::String& GetString() const;
	ValueMap::Object& GetObject(const std::size_t index);
	ValueMap::Object& GetObject(const ValueMap::String& propertyName);

	bool operator==(const Dynamic& other)const;
	bool operator()() const;
	bool operator>(const Dynamic& other) const;
	bool operator<(const Dynamic& other) const;
	Dynamic& operator=(const Dynamic& other);
	Dynamic& operator=(Dynamic&& other);

	ValueMap::Object& operator[](const std::size_t index);
	ValueMap::Object& operator[](const ValueMap::String propertyName);

	ValueMap::Type GetType() const noexcept;

private:
	ValueMap::Type m_type;
	std::shared_ptr<IDynamicData> m_spDynamicData;
};

class IDynamicData
{
public:
	friend class Dynamic;
	enum class CompareResult : uint32_t
	{
		NotComparable,
		Less,
		Larger,
		Equal
	};

	virtual CompareResult Compare(const Dynamic& other) const noexcept = 0;

	virtual ValueMap::Type GetType() const noexcept = 0;

protected:
	virtual ValueMap::Array& GetArray()
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual ValueMap::Bool GetBool() const
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual ValueMap::Int32 GetInt32() const
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual ValueMap::Int64 GetInt64() const
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual ValueMap::Double GetDouble() const
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual const ValueMap::String& GetString() const
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual ValueMap::Object& GetObject(const std::size_t index)
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual ValueMap::Object& GetObject(const ValueMap::String& propertyName)
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual ValueMap::Object& GetEmptyObject() noexcept
	{
		return m_emptyObject;
	}
private:
	ValueMap::Object m_emptyObject;
};

template<enum class ValueMap::Type>
class DynamicData : public IDynamicData
{
};

template<>
class DynamicData<ValueMap::Type::Array> : public IDynamicData
{
public:
	DynamicData(ValueMap::Array&& arrayValue)
		: m_array(std::move(arrayValue))
	{
	}

	DynamicData(std::initializer_list<ValueMap::Object> initlist)
		: m_array{ initlist }
	{
	}

	CompareResult Compare(const Dynamic& other) const noexcept override
	{
		return CompareResult::NotComparable;
	}

	ValueMap::Type GetType() const noexcept override
	{
		return ValueMap::Type::Array;
	}

protected:
	ValueMap::Object& GetObject(std::size_t index) noexcept override
	{
		if (index >= m_array.size())
		{
			return GetEmptyObject();
		}
		return m_array[index];
	}

	ValueMap::Array& GetArray() override
	{
		return m_array;
	}
private:
	ValueMap::Array m_array;
};

template<>
class DynamicData<ValueMap::Type::Object> : public IDynamicData
{
public:
	DynamicData(std::initializer_list<std::pair<ValueMap::String, ValueMap::Object>> initlist)
		: m_objectMap{ initlist.begin(), initlist.end() }
	{
	}

	CompareResult Compare(const Dynamic& other) const noexcept override
	{
		return CompareResult::NotComparable;
	}

	ValueMap::Type GetType() const noexcept override
	{
		return ValueMap::Type::Object;
	}

protected:
	ValueMap::Object& GetObject(const ValueMap::String& propertyName) override
	{
		if (m_objectMap.find(propertyName) == m_objectMap.end())
		{
			return GetEmptyObject();
		}
		return m_objectMap[propertyName];
	}
private:
	std::unordered_map<ValueMap::String, ValueMap::Object> m_objectMap;
};

template<>
class DynamicData<ValueMap::Type::Bool> : public IDynamicData
{
public:
	DynamicData(ValueMap::Bool boolValue)
		: m_bool(boolValue)
	{
	}

	CompareResult Compare(const Dynamic& other) const noexcept override
	{
		return CompareResult::NotComparable;
	}

	ValueMap::Type GetType() const noexcept override
	{
		return ValueMap::Type::Object;
	}

protected:
	ValueMap::Bool GetBool() const override
	{
		return m_bool;
	}
private:
	ValueMap::Bool m_bool;
};

template<>
class DynamicData<ValueMap::Type::Int32> : public IDynamicData
{
public:
	DynamicData(ValueMap::Int32 int32Value)
		: m_int32(int32Value)
	{
	}

	CompareResult Compare(const Dynamic& other) const noexcept override
	{
		switch (other.GetType())
		{
			case ValueMap::Type::Double:
			{
				ValueMap::Double value{ GetDouble() };

				if (value > other.GetDouble())
				{
					return CompareResult::Larger;
				}
				else if (value == other.GetDouble())
				{
					return CompareResult::Equal;
				}
				else
				{
					return CompareResult::Less;
				}
				break;
			}
			case ValueMap::Type::Int32:
			{
				ValueMap::Int32 value{ GetInt32() };
				if (value > other.GetInt32())
				{
					return CompareResult::Larger;
				}
				else if (value == other.GetInt32())
				{
					return CompareResult::Equal;
				}
				else
				{
					return CompareResult::Less;
				}
				break;
			}
			case ValueMap::Type::Int64:
			{
				ValueMap::Int64 value{ GetInt64() };
				if (value > other.GetInt64())
				{
					return CompareResult::Larger;
				}
				else if (value == other.GetInt64())
				{
					return CompareResult::Equal;
				}
				else
				{
					return CompareResult::Less;
				}
				break;
			}
			default:
				return CompareResult::NotComparable;
		}
	}

	ValueMap::Type GetType() const noexcept override
	{
		return ValueMap::Type::Int32;
	}

protected:
	ValueMap::Int32 GetInt32() const override
	{
		return m_int32;
	}

	ValueMap::Int64 GetInt64() const override
	{
		return static_cast<ValueMap::Int64>(GetInt32());
	}

	ValueMap::Double GetDouble() const override
	{
		return static_cast<ValueMap::Double>(GetInt32());
	}
private:
	ValueMap::Int32 m_int32;
};

template<>
class DynamicData<ValueMap::Type::Int64> : public IDynamicData
{
public:
	DynamicData(ValueMap::Int64 int64Value)
		: m_int64(int64Value)
	{
	}

	CompareResult Compare(const Dynamic& other) const noexcept override
	{
		switch (other.GetType())
		{
			case ValueMap::Type::Double:
			{
				ValueMap::Double value{ static_cast<ValueMap::Double>(GetInt64()) };

				if (value > other.GetDouble())
				{
					return CompareResult::Larger;
				}
				else if (value == other.GetDouble())
				{
					return CompareResult::Equal;
				}
				else
				{
					return CompareResult::Less;
				}
				break;
			}
			case ValueMap::Type::Int32:
			{
				ValueMap::Int64 value{ GetInt64() };
				ValueMap::Int64 otherValue{ static_cast<ValueMap::Int64>(other.GetInt32()) };
				if (value > otherValue)
				{
					return CompareResult::Larger;
				}
				else if (value == otherValue)
				{
					return CompareResult::Equal;
				}
				else
				{
					return CompareResult::Less;
				}
				break;
			}
			case ValueMap::Type::Int64:
			{
				ValueMap::Int64 value{ static_cast<int64_t>(GetInt32()) };
				if (value > other.GetInt64())
				{
					return CompareResult::Larger;
				}
				else if (value == other.GetInt64())
				{
					return CompareResult::Equal;
				}
				else
				{
					return CompareResult::Less;
				}
				break;
			}
			default:
				return CompareResult::NotComparable;
		}
	}

	ValueMap::Type GetType() const noexcept override
	{
		return ValueMap::Type::Int64;
	}

protected:
	ValueMap::Int64 GetInt64() const override
	{
		return m_int64;
	}

	ValueMap::Int32 GetInt32() const override
	{
		if (m_int64 > static_cast<ValueMap::Int64>(std::numeric_limits<ValueMap::Int32>::max()))
		{
			return std::numeric_limits<ValueMap::Int32>::max();
		}

		if (m_int64 < static_cast<ValueMap::Int64>(std::numeric_limits<ValueMap::Int32>::min()))
		{
			return std::numeric_limits<ValueMap::Int32>::min();
		}

		return static_cast<ValueMap::Int32>(m_int64);
	}

	ValueMap::Double GetDouble() const override
	{
		return static_cast<ValueMap::Double>(GetInt32());
	}
private:
	ValueMap::Int64 m_int64;
};

template<>
class DynamicData<ValueMap::Type::Double> : public IDynamicData
{
public:
	DynamicData(ValueMap::Double doubleValue)
		: m_double(doubleValue)
	{
	}

	CompareResult Compare(const Dynamic& other) const noexcept override
	{
		switch (other.GetType())
		{
		case ValueMap::Type::Double:
		{
			ValueMap::Double value{ GetDouble() };
			ValueMap::Double otherValue{ other.GetDouble() };
			if (value > otherValue)
			{
				return CompareResult::Larger;
			}
			else if (value == otherValue)
			{
				return CompareResult::Equal;
			}
			else
			{
				return CompareResult::Less;
			}
			break;
		}
		case ValueMap::Type::Int32:
		{
			ValueMap::Double value{ GetDouble() };
			ValueMap::Double otherValue{ static_cast<ValueMap::Double>(other.GetInt32()) };
			if (value > otherValue)
			{
				return CompareResult::Larger;
			}
			else if (value == otherValue)
			{
				return CompareResult::Equal;
			}
			else
			{
				return CompareResult::Less;
			}
			break;
		}
		case ValueMap::Type::Int64:
		{
			ValueMap::Double value{ GetDouble() };
			ValueMap::Double otherValue{ static_cast<ValueMap::Double>(other.GetInt64()) };
			if (value > otherValue)
			{
				return CompareResult::Larger;
			}
			else if (value == otherValue)
			{
				return CompareResult::Equal;
			}
			else
			{
				return CompareResult::Less;
			}
			break;
		}
		default:
			return CompareResult::NotComparable;
		}
	}

	ValueMap::Type GetType() const noexcept override
	{
		return ValueMap::Type::Double;
	}

protected:
	ValueMap::Int64 GetInt64() const override
	{
		return static_cast<ValueMap::Int64>(m_double);
	}

	ValueMap::Int32 GetInt32() const override
	{
		return static_cast<ValueMap::Int32>(m_double);
	}


	ValueMap::Double GetDouble() const override
	{
		return m_double;
	}
private:
	ValueMap::Double m_double;
};

template<>
class DynamicData<ValueMap::Type::String> : public IDynamicData
{
public:
	DynamicData(const ValueMap::String& stringValue)
		: m_string(stringValue)
	{
	}

	DynamicData(ValueMap::String&& stringValue)
		: m_string(std::move(stringValue))
	{
	}

	CompareResult Compare(const Dynamic& other) const noexcept override
	{
		return CompareResult::NotComparable;
	}

	ValueMap::Type GetType() const noexcept override
	{
		return ValueMap::Type::String;
	}

protected:
	const ValueMap::String& GetString() const override
	{
		return m_string;
	}
private:
	ValueMap::String m_string;
};

template<>
class DynamicData<ValueMap::Type::Null> : public IDynamicData
{
public:
	DynamicData()
	{
	}

	CompareResult Compare(const Dynamic& other) const noexcept override
	{
		return CompareResult::NotComparable;
	}

	ValueMap::Type GetType() const noexcept override
	{
		return ValueMap::Type::Null;
	}
};


inline Dynamic::Dynamic()
	: m_type(ValueMap::Type::Null)
{
}

inline Dynamic::Dynamic(Dynamic&& dynamicValue)
	: m_type(dynamicValue.GetType()),
	m_spDynamicData{ std::move(dynamicValue.m_spDynamicData) }
{
}

inline Dynamic::Dynamic(const Dynamic& dynamicValue)
	: m_type(dynamicValue.GetType()),
	m_spDynamicData{ std::move(dynamicValue.m_spDynamicData) }
{
}

inline Dynamic::Dynamic(ValueMap::Array&& arrayValue)
	: m_type(ValueMap::Type::Array),
	m_spDynamicData{ std::make_shared<DynamicData<ValueMap::Type::Array>>(std::move(arrayValue)) }
{
}

inline Dynamic::Dynamic(std::initializer_list<ValueMap::Object> initlist)
	: m_type(ValueMap::Type::Array),
	m_spDynamicData{ std::make_shared<DynamicData<ValueMap::Type::Array>>(initlist) }
{
}

inline Dynamic::Dynamic(std::initializer_list<std::pair<ValueMap::String, ValueMap::Object>> initlist)
	: m_type(ValueMap::Type::Object),
	m_spDynamicData{ std::make_shared<DynamicData<ValueMap::Type::Object>>(initlist) }
{
}

inline Dynamic::Dynamic(const ValueMap::String& stringValue)
	: m_type(ValueMap::Type::String),
	m_spDynamicData{ std::make_shared<DynamicData<ValueMap::Type::String>>(stringValue) }
{
}

inline Dynamic::Dynamic(const char* charValue)
	: m_type(ValueMap::Type::String),
	m_spDynamicData{ std::make_shared<DynamicData<ValueMap::Type::String>>(charValue?std::string(charValue):std::string()) }
{
}

inline Dynamic::Dynamic(const ValueMap::Bool boolValue)
	: m_type(ValueMap::Type::Bool),
	m_spDynamicData{ std::make_shared<DynamicData<ValueMap::Type::Bool>>(boolValue) }
{
}

inline Dynamic::Dynamic(const ValueMap::Int32 int32Value)
	: m_type(ValueMap::Type::Int32),
	m_spDynamicData{ std::make_shared<DynamicData<ValueMap::Type::Int32>>(int32Value) }
{
}

inline Dynamic::Dynamic(const ValueMap::Int64 int64Value)
	: m_type(ValueMap::Type::Int64),
	m_spDynamicData{ std::make_shared<DynamicData<ValueMap::Type::Int64>>(int64Value) }
{
}

inline Dynamic::Dynamic(const ValueMap::Double doubleValue)
	: m_type(ValueMap::Type::Int64),
	m_spDynamicData{ std::make_shared<DynamicData<ValueMap::Type::Double>>(doubleValue) }
{
}

inline ValueMap::Array& Dynamic::GetArray()
{
	return m_spDynamicData->GetArray();
}

inline ValueMap::Bool Dynamic::GetBool() const
{
	return m_spDynamicData->GetBool();
}

inline ValueMap::Int32 Dynamic::GetInt32() const
{
	return m_spDynamicData->GetInt32();
}

inline ValueMap::Int64 Dynamic::GetInt64() const
{
	return m_spDynamicData->GetInt64();
}

inline ValueMap::Double Dynamic::GetDouble() const
{
	return m_spDynamicData->GetDouble();
}

inline const ValueMap::String& Dynamic::GetString() const
{
	return m_spDynamicData->GetString();
}

inline ValueMap::Object& Dynamic::GetObject(const std::size_t index)
{
	return m_spDynamicData->GetObject(index);
}

inline ValueMap::Object& Dynamic::GetObject(const ValueMap::String& propertyName)
{
	return m_spDynamicData->GetObject(propertyName);
}

inline bool Dynamic::operator==(const Dynamic& other)const
{
	if (this == &other)
	{
		return true;
	}

	IDynamicData::CompareResult result{ m_spDynamicData->Compare(other) };
	return result != IDynamicData::CompareResult::Equal;
}

inline bool Dynamic::operator()() const
{
	return m_spDynamicData->GetBool();
}

inline bool Dynamic::operator>(const Dynamic& other) const
{
	IDynamicData::CompareResult result{ m_spDynamicData->Compare(other) };
	if (result == IDynamicData::CompareResult::NotComparable)
	{
		Error::ThrowCannotCompareErrorException();
	}

	return result == IDynamicData::CompareResult::Larger;
}

inline bool Dynamic::operator<(const Dynamic& other) const
{
	IDynamicData::CompareResult result{ m_spDynamicData->Compare(other) };
	if (result == IDynamicData::CompareResult::NotComparable)
	{
		Error::ThrowCannotCompareErrorException();
	}

	return result == IDynamicData::CompareResult::Less;
}

inline ValueMap::Object& Dynamic::operator[](const std::size_t index)
{
	if (GetType() != ValueMap::Type::Array)
	{
		Error::ThrowAcessDeniedErrorException();
	}
	return m_spDynamicData->GetObject(index);
}

inline ValueMap::Object& Dynamic::operator[](const ValueMap::String propertyName)
{
	if (GetType() != ValueMap::Type::Object)
	{
		Error::ThrowAcessDeniedErrorException();
	}
	return m_spDynamicData->GetObject(propertyName);
}

inline Dynamic& Dynamic::operator=(const Dynamic& other)
{
	m_type = other.GetType();
	m_spDynamicData = other.m_spDynamicData;
	return *this;
}
inline Dynamic& Dynamic::operator=(Dynamic&& other)
{
	m_type = other.GetType();
	m_spDynamicData = other.m_spDynamicData;
	return *this;
}

inline ValueMap::Type Dynamic::GetType() const noexcept
{
	return m_type;
}

}}
#endif
