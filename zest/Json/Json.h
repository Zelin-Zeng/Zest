#pragma once

#ifndef ZEST_LIB_JSON_H
#define ZEST_LIB_JSON_H

#include <string>
#include <unordered_map>
#include <type_traits>
#include "../Error.h"

namespace Zest { namespace Lib { namespace Json {

enum JsonValueType
{
	Null,
	Integer,
	String,
	Boolean,
	Double,
	Object,
	Array
};

struct IJsonValue;
class JsonObject;
class JsonArray;

template<JsonValueType, typename TValue = void>
struct JsonValueTypeMap;

template<>
struct JsonValueTypeMap<JsonValueType::Boolean>
{
	using Type = bool;
};

template<>
struct JsonValueTypeMap<JsonValueType::Integer>
{
	using Type = long long;
};

template<>
struct JsonValueTypeMap<JsonValueType::Double>
{
	using Type = double;
};

template<>
struct JsonValueTypeMap<JsonValueType::String>
{
	using Type = std::string;
};

template<>
struct JsonValueTypeMap<JsonValueType::Object>
{
	using Type = JsonObject;
};

template<>
struct JsonValueTypeMap<JsonValueType::Array>
{
	using Type = JsonArray;
};

template<>
struct JsonValueTypeMap<JsonValueType::Null>
{
	using Type = void;
};

template<typename T>
struct RVToJVMap;

template<>
struct RVToJVMap<int>
{
	using Type = JsonValue<JsonValueType::Integer>;
};

template<>
struct RVToJVMap<int>
{
	using Type = JsonValue<JsonValueType::Integer>;
};

template<>
struct RVToJVMap<long>
{
	using Type = JsonValue<JsonValueType::Integer>;
};

template<>
struct RVToJVMap<bool>
{
	using Type = JsonValue<JsonValueType::Boolean>;
};

template<>
struct RVToJVMap<std::string>
{
	using Type = JsonValue<JsonValueType::String>;
};

template<>
struct RVToJVMap<const char*>
{
	using Type = JsonValue<JsonValueType::String>;
};

template<>
struct RVToJVMap<double>
{
	using Type = JsonValue<JsonValueType::Double>;
};

template<>
struct RVToJVMap<JsonObject>
{
	using Type = JsonValue<JsonValueType::Object>;
};

template<>
struct RVToJVMap<JsonArray>
{
	using Type = JsonValue<JsonValueType::Array>;
};

template<>
struct RVToJVMap<void>
{
	using Type = JsonValue<JsonValueType::Null>;
};
// Null Value
struct TNull {};
const struct TNull null_t {};

class JsonObject
{
public:
	JsonObject(const std::string& jsonString);
	JsonObject(JsonObject&& obj);
	JsonObject(const JsonObject& obj);

	std::shared_ptr<const IJsonValue> GetValue(const std::string& property)
	{
		if (m_objectMap.find(property) != m_objectMap.end())
		{
			return m_objectMap[property];
		}

		return nullptr;
	}

	template<typename T>
	void SetValue(const std::string& property, const T& value)
	{
		std::shared_ptr<RVToJVMap<T>> spValue{ std::make_shared<RVToJVMap<T>>(value) };
		m_objectMap[property] = spValue;
	}

	void SetValue(const std::string& property, std::shared_ptr<IJsonValue> spJsonValue)
	{
		m_objectMap[property] = spJsonValue;
	}
private:
	std::unordered_map<std::string, std::shared_ptr<IJsonValue>> m_objectMap;
};

class JsonArray
{
public:
	JsonArray(const std::string& jsonString);

	JsonObject& GetObject(std::size_t index);
	void Set(std::size_t index, const JsonObject& obj);
	void Append(const JsonObject& obj);

private:
	std::vector<JsonObject> m_objectArray;
};

struct IJsonValue
{
	virtual JsonValueType GetType() = 0;

	virtual typename JsonValueTypeMap<JsonValueType::Boolean>::Type GetBoolean()
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual typename JsonValueTypeMap<JsonValueType::Integer>::Type GetInteger()
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual typename JsonValueTypeMap<JsonValueType::String>::Type& GetString()
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual typename JsonValueTypeMap<JsonValueType::Double>::Type GetDouble()
	{
		Error::ThrowAcessDeniedErrorException();
	}

	virtual typename JsonValueTypeMap<JsonValueType::Object>::Type GetObject()
	{
		Error::ThrowAcessDeniedErrorException();
	}
};

template<JsonValueType valueType, bool NotNull = std::integral_constant<bool, valueType != JsonValueType::Null>::value>
class JsonValue : public IJsonValue
{
public:
	JsonValue(const typename JsonValueTypeMap<valueType>::Type& value)
		: m_value{ value }
	{
	}

	JsonValueType GetType()
	{
		return valueType;
	}

	template<typename=std::enable_if_t<valueType == JsonValueType::Boolean>>
	typename JsonValueTypeMap<JsonValueType::Boolean>::Type GetBoolean()
	{
		return m_value;
	}

	template<typename = std::enable_if_t<valueType == JsonValueType::Integer>>
	typename JsonValueTypeMap<JsonValueType::Integer>::Type GetInteger()
	{
		return m_value;
	}

	template<typename = std::enable_if_t<valueType == JsonValueType::String>>
	typename JsonValueTypeMap<JsonValueType::String>::Type& GetString()
	{
		return m_value;
	}

	template<typename = std::enable_if_t<valueType == JsonValueType::Double>>
	typename JsonValueTypeMap<JsonValueType::Double>::Type GetDouble()
	{
		return m_value;
	}

	template<typename = std::enable_if_t<valueType == JsonValueType::Object>>
	typename JsonValueTypeMap<JsonValueType::Object>::Type& GetObject()
	{
		return m_value;
	}

	bool operator==(const JsonValue<valueType>& other)
	{
		return m_value == other.m_value;
	}

	bool operator!=(const JsonValue<valueType>& other)
	{
		return m_value != other.m_value;
	}

private:
	typename JsonValueTypeMap<valueType>::Type m_value;
};

template<>
class JsonValue<JsonValueType::Null, false> : public IJsonValue
{
public:
	JsonValue()
	{
	}

	JsonValueType GetType()
	{
		return JsonValueType::Null;
	}

	bool operator==(const JsonValue<JsonValueType::Null>& other)
	{
		return true;
	}

	bool operator!=(const JsonValue<JsonValueType::Null>& other)
	{
		return false;
	}

	bool operator==(TNull)
	{
		return true;
	}

	bool operator!=(TNull)
	{
		return false;
	}
};


/*! Stream to store json data.
	Please don't  destruct orginal string when stream is in use.
*/
class JsonStringStream
{
public:
	JsonStringStream(const std::string& jsonString)
		: m_begin{ jsonString.c_str() }, m_cur{ jsonString.c_str() },
		m_end{ jsonString.c_str() + jsonString.length() },
		m_size{ jsonString.length() }
	{
	}

	JsonStringStream(const char* src, size_t size)
		: m_begin{ src }, m_cur{ src },
		m_end{ src + size },
		m_size{ size }
	{
	}

	char Peek()
	{
		assert(m_cur != m_end);
		return *m_cur;
	}

	void Skip(size_t skipSize)
	{
		assert(m_cur + skipSize <= m_end);
		m_cur += skipSize;
	}

	bool IsEnd()
	{
		return m_cur == m_end;
	}

	char Read()
	{
		char c{ Peek() };
		Skip(1);
		return c;
	}

private:
	const char* m_cur;
	const char* m_begin;
	const char* m_current;
	const char* m_end;
	size_t m_size;
};

enum class JsonErrorCode: uint32_t
{
	Parse_EmptyBody,
	Parse_Malformat,
};

class JsonError: public std::exception
{
public:
	JsonError(const JsonErrorCode errorCode, const char* errorMsg)
		: m_errorCode{ errorCode }, m_errorMsg{ errorMsg ? errorMsg : "" }
	{
	}

	const char* what() const noexcept
	{
		return m_errorMsg.c_str();
	}

	JsonErrorCode GetErrorCode() const
	{
		return m_errorCode;
	}
private:
	JsonErrorCode m_errorCode;
	std::string m_errorMsg;
};

class JsonParser
{
public:
	static std::shared_ptr<IJsonValue> ParseJson(const std::string& jsonString)
	{
		return ParseJson(jsonString.c_str(), jsonString.size());
	}

	static std::shared_ptr<IJsonValue> ParseJson(const char* jsonString)
	{
		return ParseJson(jsonString, strlen(jsonString));
	}

	static std::shared_ptr<IJsonValue> ParseJson(const char* jsonString, size_t size)
	{
		JsonStringStream jsonStream{ jsonString, size };
		SkipWhiteAndNewLine(jsonStream);
		if (jsonStream.IsEnd())
		{
			throw JsonError{ JsonErrorCode::Parse_EmptyBody, "Json Input is empty." };
		}

		char c{ jsonStream.Peek() };

		if (c == '{') // if object
		{
			return  ParseObject(jsonStream);
		}
		else if (c == '[') // if array
		{
			return ParseArray(jsonStream);
		}
		else if ((c == 'n' || c == 'N') && CheckIsNull(jsonStream))
		{
			return nullptr;
		}
		else
		{
			std::string errorMsg{ "Unknown start of json: " };
			errorMsg += c;
			throw JsonError{JsonErrorCode::Parse_Malformat, errorMsg.c_str() };
		}

		std::string errorMsg{ "Cannot parse content: " };
		errorMsg += jsonString;
		throw JsonError{ JsonErrorCode::Parse_Malformat, errorMsg.c_str() };
	}
private:

	static std::shared_ptr<IJsonValue> ParseObject(JsonStringStream& jss)
	{
		SkipWhiteAndNewLine(jss);
		assert(jss.Peek() == '{');


	}

	static std::shared_ptr<IJsonValue> ParseArray(JsonStringStream& jss)
	{
		SkipWhiteAndNewLine(jss);
		assert(jss.Peek() == '[');
	}


	static void SkipWhiteAndNewLine(JsonStringStream& jss)
	{
		char c;
		while (!jss.IsEnd() && ((c = jss.Peek()) == ' ' || c == '\n' || c == '\r' || c == '\t'))
		{
			jss.Skip(1);
		}
		return;
	}

	static bool CheckIsNull(JsonStringStream& jss) noexcept
	{
		SkipWhiteAndNewLine(jss);
		char c;
		c = jss.Peek();
		if (c != 'N' && c != 'n')
			return false;
		jss.Skip(1);
		if (c != 'U' && c != 'u')
			return false;
		jss.Skip(1);
		if (c != 'l' && c != 'l')
			return false;
		jss.Skip(1);
		if (c != 'l' && c != 'l')
			return false;
		jss.Skip(1);

		return true;
	}
};

}}}

#endif


