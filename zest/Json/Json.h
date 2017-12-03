#pragma once
#ifndef ZEST_LIB_JSON_JSON_H
#define ZEST_LIB_JSON_JSON_H

#include <string>
#include <unordered_map>

#include "..\Optional.h"

namespace Zest { namespace Lib { namespace Json {

// Json object type
class JsonArray;
class JsonObject;

enum class JsonValueType
{
	String,
	Array,
	Bool,
	Null,
	Int,
	Long,
	Double,
	Error
};

struct JsonData
{
	using JsonPropertyName = const std::string&;
	using JsonPropertyStringValue = JsonPropertyName;
	using JsonString = const std::string&;
	using JsonMapPropertyName = std::string;
};

namespace Details {

struct JsonValueRealType
{
	using JsonString = std::string;
	using JsonArray = std::vector<JsonObject>;
	using JsonObject = std::unordered_map<JsonString, JsonObject>;
	using JsonInt32 = int32_t;
	using JsonInt64 = int64_t;
	using JsonBool = bool;
	using JsonDouble = double;
	using JsonError = std::unique_ptr<Error::IError>;
};

class IJsonValue
{
public:
	friend class JsonObject;

protected:
	virtual JsonValueType GetType() const noexcept = 0;
	virtual Optional<JsonValueRealType::JsonString> AsString() const noexcept
	{
		return Zest::Lib::nullopt;
	}

	virtual Optional<JsonValueRealType::JsonArray> AsArray() noexcept
	{
		return Zest::Lib::nullopt;
	}

	virtual Optional<JsonValueRealType::JsonObject> AsObject() noexcept
	{
		return Zest::Lib::nullopt;
	}

	virtual Optional<JsonValueRealType::JsonBool> AsBool() const noexcept
	{
		return Zest::Lib::nullopt;
	}

	virtual Optional<JsonValueRealType::JsonDouble> AsDouble() const noexcept
	{
		return Zest::Lib::nullopt;
	}

	virtual Optional<JsonValueRealType::JsonInt32> AsInt32() const noexcept
	{
		return Zest::Lib::nullopt;
	}

	virtual Optional<JsonValueRealType::JsonInt64> AsInt64() const noexcept
	{
		return Zest::Lib::nullopt;
	}

	virtual Optional<JsonValueRealType::JsonError> AsError() const noexcept
	{
		return Zest::Lib::nullopt;
	}
};


template<enum class JsonValueType>
class JsonValue :public IJsonValue
{
};

template<>
class JsonValue<JsonValueType::Array> : public IJsonValue
{
public:
	JsonValue(const JsonValueRealType::JsonArray& jsonArray)
		: m_jsonArray{ jsonArray.begin(), jsonArray.end() }
	{
	}

	JsonValue(JsonValueRealType::JsonArray&& jsonArray)
		: m_jsonArray{ std::move(jsonArray) }
	{
	}

	JsonValue(JsonValue<JsonValueType::Array>&& otherValue)
		: m_jsonArray{ std::move(otherValue.m_jsonArray) }
	{
	}

	JsonValue(const JsonValue<JsonValueType::Array>& otherValue)
		: m_jsonArray{ otherValue.m_jsonArray.begin(), otherValue.m_jsonArray.end() }
	{
	}
protected:
	JsonValueType GetType() const noexcept override
	{
		return JsonValueType::Array;
	}

	Optional<JsonValueRealType::JsonArray> AsArray() noexcept override
	{
		return m_jsonArray;
	}
private:
	JsonValueRealType::JsonArray m_jsonArray;
};

template<>
class JsonValue<JsonValueType::Bool> : public IJsonValue
{
public:
	JsonValue(const JsonValueRealType::JsonBool jsonBool)
		: m_jsonBool{ jsonBool }
	{
	}

	JsonValue(const JsonValue<JsonValueType::Bool>& otherValue)
		: m_jsonBool{ otherValue.m_jsonBool }
	{
	}

	JsonValue(JsonValue<JsonValueType::Bool>&& otherValue)
		: m_jsonBool{ otherValue.m_jsonBool }
	{
	}
protected:
	JsonValueType GetType() const noexcept override
	{
		return JsonValueType::Array;
	}

	Optional<JsonValueRealType::JsonBool> AsBool() const noexcept override
	{
		return m_jsonBool;
	}
private:
	JsonValueRealType::JsonBool m_jsonBool;
};

template<>
class JsonValue<JsonValueType::String> : public IJsonValue
{
public:
	JsonValue(const JsonValueRealType::JsonString& jsonString)
		: m_jsonString{ jsonString }
	{
	}

	JsonValue(const char* charValue)
		: m_jsonString{ charValue ? charValue : JsonValueRealType::JsonString() }
	{
	}

	JsonValue(const JsonValue<JsonValueType::String>& otherValue)
		: m_jsonString{ otherValue.m_jsonString }
	{
	}

	JsonValue(JsonValue<JsonValueType::String>&& otherValue)
		: m_jsonString{ std::move(otherValue.m_jsonString) }
	{
	}
protected:
	JsonValueType GetType() const noexcept override
	{
		return JsonValueType::Array;
	}

	Optional<JsonValueRealType::JsonString> AsString() const noexcept override
	{
		return m_jsonString;
	}
private:
	JsonValueRealType::JsonString m_jsonString;
};

template<>
class JsonValue<JsonValueType::Int> : public IJsonValue
{
public:
	JsonValue(const JsonValueRealType::JsonInt32 jsonInt32)
		: m_jsonInt32{ jsonInt32 }
	{
	}

	JsonValue(const JsonValue<JsonValueType::Int>& otherValue)
		: m_jsonInt32{ otherValue.m_jsonInt32 }
	{
	}

	JsonValue(JsonValue<JsonValueType::Int>&& otherValue)
		: m_jsonInt32{ otherValue.m_jsonInt32 }
	{
	}
protected:
	JsonValueType GetType() const noexcept override
	{
		return JsonValueType::Array;
	}

	Optional<JsonValueRealType::JsonDouble> AsDouble() const noexcept override
	{
		return static_cast<JsonValueRealType::JsonDouble>(m_jsonInt32);
	}

	Optional<JsonValueRealType::JsonInt32> AsInt32() const noexcept override
	{
		return m_jsonInt32;
	}

	Optional<JsonValueRealType::JsonInt64> AsInt64() const noexcept override
	{
		return static_cast<JsonValueRealType::JsonInt64>(m_jsonInt32);
	}
private:
	JsonValueRealType::JsonInt32 m_jsonInt32;
};

template<>
class JsonValue<JsonValueType::Long> : public IJsonValue
{
public:
	JsonValue(const JsonValueRealType::JsonInt64 jsonInt64)
		: m_jsonInt64{ jsonInt64 }
	{
	}

	JsonValue(const JsonValue<JsonValueType::Long>& otherValue)
		: m_jsonInt64{ otherValue.m_jsonInt64 }
	{
	}

	JsonValue(JsonValue<JsonValueType::Long>&& otherValue)
		: m_jsonInt64{ otherValue.m_jsonInt64 }
	{
	}
protected:
	JsonValueType GetType() const noexcept override
	{
		return JsonValueType::Array;
	}

	Optional<JsonValueRealType::JsonInt64> AsInt64() const noexcept override
	{
		return m_jsonInt64;
	}

	Optional<JsonValueRealType::JsonDouble> AsDouble() const noexcept override
	{
		return static_cast<JsonValueRealType::JsonDouble>(m_jsonInt64);
	}

	Optional<JsonValueRealType::JsonInt32> AsInt32() const noexcept override
	{
		return static_cast<JsonValueRealType::JsonInt32>(m_jsonInt64);
	}
private:
	JsonValueRealType::JsonInt64 m_jsonInt64;
};

template<>
class JsonValue<JsonValueType::Double> : public IJsonValue
{
public:
	JsonValue(const JsonValueRealType::JsonDouble jsonInt64)
		: m_jsonDouble{ jsonInt64 }
	{
	}

	JsonValue(const JsonValue<JsonValueType::Double>& otherValue)
		: m_jsonDouble{ otherValue.m_jsonDouble }
	{
	}

	JsonValue(JsonValue<JsonValueType::Double>&& otherValue)
		: m_jsonDouble{ otherValue.m_jsonDouble }
	{
	}
protected:
	JsonValueType GetType() const noexcept override
	{
		return JsonValueType::Double;
	}

	Optional<JsonValueRealType::JsonDouble> AsDouble() const noexcept override
	{
		return m_jsonDouble;
	}

	Optional<JsonValueRealType::JsonInt32> AsInt32() const noexcept override
	{
		return static_cast<JsonValueRealType::JsonInt32>(m_jsonDouble);
	}

	Optional<JsonValueRealType::JsonInt64> AsInt64() const noexcept override
	{
		return static_cast<JsonValueRealType::JsonInt64>(m_jsonDouble);
	}
private:
	JsonValueRealType::JsonDouble m_jsonDouble;
};

template<>
class JsonValue<JsonValueType::Error> : public IJsonValue
{
public:
	JsonValue(JsonValueRealType::JsonError jsonError)
		: m_jsonError{ std::move(jsonError) }
	{
	}

	JsonValue(JsonValue<JsonValueType::Error>&& otherValue)
		: m_jsonError{ std::move(otherValue.m_jsonError) }
	{
	}
protected:
	JsonValueType GetType() const noexcept override
	{
		return JsonValueType::Array;
	}

	Optional<JsonValueRealType::JsonError> AsError() const noexcept
	{
		return m_jsonError;
	}
private:
	JsonValueRealType::JsonError m_jsonError;
};

template<>
class JsonValue<JsonValueType::Null> : public IJsonValue
{
public:
	JsonValue() = default;
protected:
	JsonValueType GetType() const noexcept override
	{
		return JsonValueType::Null;
	}
};

}

class JsonObject
{
public:
	JsonObject(JsonData::JsonString jsonString);
	JsonObject();
	JsonObject(JsonObject&& other);

	Optional<JsonObject> GetJsonObject(JsonData::JsonPropertyName objName) const;
	Optional<JsonArray> GetJsonArray(JsonData::JsonPropertyName arrayName) const;
	Optional<int32_t> GetInt32(JsonData::JsonPropertyName propName) const;
	Optional<int64_t> GetInt64(JsonData::JsonPropertyName propName) const;
	Optional<JsonData::JsonPropertyStringValue> GetString(JsonData::JsonPropertyName propName) const;
	Optional<double> GetDouble(JsonData::JsonPropertyName propName) const;
	Optional<bool> GetBool(JsonData::JsonPropertyName propName) const;

	template<typename TValue>
	void Put(JsonPropertyName propName, TValue value) const;
private:
	std::unordered_map<JsonData::JsonMapPropertyName, std::unique_ptr<Details::IJsonValue>> m_jsonMap;
};

}}}

#endif