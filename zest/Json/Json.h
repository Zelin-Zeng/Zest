#pragma once
#ifndef ZEST_LIB_JSON_JSON_H
#define ZEST_LIB_JSON_JSON_H

#include <string>
#include <unordered_map>

namespace Zest { namespace Lib { namespace Json {

struct JsonData
{
	using JsonPropertyName = const std::string&;
	using JsonPropertyStringValue = JsonPropertyName;
	using JsonString = const std::string&;
	using JsonMapPropertyName = std::string;
};

// Json type
class JsonArray;
class JsonValue;

class JsonObject
{
public:
	JsonObject(JsonData::JsonString jsonString);
	JsonObject();
	JsonObject(JsonObject&& other);

	JsonObject GetJsonObject(JsonData::JsonPropertyName objName) const;
	JsonArray GetJsonArray(JsonData::JsonPropertyName arrayName) const;
	int32_t GetInt32(JsonData::JsonPropertyName propName) const;
	uint32_t GetUInt32(JsonData::JsonPropertyName propName) const;
	int64_t GetInt64(JsonData::JsonPropertyName propName) const;
	uint64_t GetUInt64(JsonData::JsonPropertyName propName) const;
	JsonData::JsonPropertyStringValue GetString(JsonData::JsonPropertyName propName) const;
	double GetDouble(JsonData::JsonPropertyName propName) const;
	bool GetBool(JsonData::JsonPropertyName propName) const;

	template<typename TValue>
	void Put(JsonPropertyName propName, TValue value) const;
private:
	std::unordered_map<JsonData::JsonMapPropertyName, JsonValue> m_jsonMap;
};

}}}

#endif