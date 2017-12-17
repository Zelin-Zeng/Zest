#pragma once

#ifndef ZEST_LIB_JSON_H
#define ZEST_LIB_JSON_H

#include <string>
#include <unordered_map>

namespace Zest { namespace Lib { namespace Json {

enum JsonValueType
{
	Null,
	Integer,
	String,
	Boolean,
	Double,
};

// Null Value
struct TNull {};
const struct TNull null_t {};

class IJsonValue;

template<typename T>
IJsonValue GetJsonValue(const T& value);

class JsonObject
{
public:
	JsonObject(const std::string& jsonString);
	JsonObject(JsonObject&& obj);

	const IJsonValue& GetValue(const std::string& property);

	template<typename T>
	void SetValue(const std::string& property, const T& value);
private:
	std::unordered_map<std::string, IJsonValue> m_objectMap;
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




}}}

#endif


