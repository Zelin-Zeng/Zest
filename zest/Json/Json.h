#pragma once
#ifndef ZEST_LIB_JSON_JSON_H
#define ZEST_LIB_JSON_JSON_H

#include <string>
#include <unordered_map>

namespace Zest { namespace Lib { namespace Json {

using JsonPropertyName = const std::string&;
using JsonPropertyStringValue = JsonPropertyName;
using JsonString = const std::string&;
using JsonMapPropertyName = std::string;

// Json type
class JsonArray;
class JsonValue;

class JsonObject
{
public:
	JsonObject(JsonString jsonString);
	JsonObject();
	JsonObject(JsonObject&& other);

	JsonObject GetJsonObject(JsonPropertyName objName) const;
	JsonArray GetJsonArray(JsonPropertyName arrayName) const;
	int32_t GetInt32(JsonPropertyName propName) const;
	uint32_t GetUInt32(JsonPropertyName propName) const;
	int64_t GetInt64(JsonPropertyName propName) const;
	uint64_t GetUInt64(JsonPropertyName propName) const;
	JsonPropertyStringValue GetString(JsonPropertyName propName) const;
	double GetDouble(JsonPropertyName propName) const;
	bool GetBool(JsonPropertyName propName) const;

	template<typename TValue>
	void Put(JsonPropertyName propName, TValue value) const;
private:
	std::unordered_map<JsonMapPropertyName, JsonValue> m_jsonMap;
};

}}}

#endif