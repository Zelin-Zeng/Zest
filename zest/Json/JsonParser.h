#pragma once
#ifndef ZEST_LIB_JSON_JSONPARSER_H
#define ZEST_LIB_JSON_JSONPARSER_H

#include "Json.h"
#include "..\Maybe.h"
#include <queue>
#include <sstream>

namespace Zest { namespace Lib { namespace Json {

struct IJsonParser
{
	virtual JsonObject ParseObject(JsonData::JsonString jsonString) noexcept = 0;
	virtual JsonArray ParseArray(JsonData::JsonString jsonString) noexcept = 0;
};

class LazyJsonParser : public IJsonParser
{
public:
	JsonObject ParseObject(JsonData::JsonString jsonString) noexcept;
	JsonArray ParseArray(JsonData::JsonString jsonString) noexcept;
};

class JsonStringBuffer
{
public:
	JsonStringBuffer(const std::string& jsonString) noexcept
	{
		for (char c : jsonString)
		{
			m_charQueue.push(c);
		}
	}

	void SkipWhiteAndNewLine() noexcept
	{
		while (!m_charQueue.empty() && Utils::IsWhiteOrNewLine(m_charQueue.front()))
		{
			m_charQueue.pop();
		}
	}

	char Get() noexcept
	{
		if (m_charQueue.empty())
		{
			return '\0';
		}

		return m_charQueue.front();
	}

	char Take() noexcept
	{
		if (m_charQueue.empty())
		{
			return '\0';
		}

		char  c{ m_charQueue.front() };
		m_charQueue.pop();
		return c;
	}

	std::size_t Skip(std::size_t skipLength) noexcept
	{
		if (skipLength >= m_charQueue.size())
		{
			skipLength = m_charQueue.size();
			std::queue<char>().swap(m_charQueue);
			return skipLength;
		}

		size_t i{ 0 };
		while (i < skipLength)
		{
			m_charQueue.pop();
			i++;
		}
		return skipLength;
	}

	Maybe<std::string> GetNextValueString() noexcept
	{
		SkipWhiteAndNewLine();
		if (Take() != ':')
		{
			return Maybe<std::string>(Error::MakeError(Error::ErrorCode::MalFormatError));
		}

		SkipWhiteAndNewLine();
		switch (Get())
		{
		case '"':
			return GetNextStringString();
		case 'T':
		case 'F':
		case 'N':
		case 't':
		case 'f':
		case 'n':
			return GetNextNonStringValueString();
		}

		return Maybe<std::string>(Error::MakeError(Error::ErrorCode::MalFormatError));
	}

	std::size_t Size() noexcept
	{
		return m_charQueue.size();
	}

	bool IsEnd() noexcept
	{
		return m_charQueue.empty();
	}

private:
	Maybe<std::string> GetNextStringString() noexcept
	{
		std::stringstream ss;
		
	}

	Maybe<std::string> GetNextNonStringValueString() noexcept
	{

	}

	std::queue<char> m_charQueue;
};

namespace Utils
{
enum class KeyCharType
{
	StartOfObject,
	EndOfObject,
	StartOfArray,
	EndOfArray,
	KeyValueSplitter,
	EntrySplitter,
	Unknown
};

KeyCharType FindKeyCharType(char c) noexcept
{
	switch (c)
	{
	case '{':
		return KeyCharType::StartOfObject;
	case '}':
		return KeyCharType::EndOfObject;
	case '[':
		return KeyCharType::StartOfArray;
	case ']':
		return KeyCharType::EndOfArray;
	case '.':
		return KeyCharType::EntrySplitter;
	case ':':
		return KeyCharType::KeyValueSplitter;
	default:
		break;
	}
	return KeyCharType::Unknown;
}

inline bool IsWhiteOrNewLine(char c) noexcept
{
	return c == ' ' || c == '\r' || c == '\t' || c == '\n';
}

}

}}}

#endif