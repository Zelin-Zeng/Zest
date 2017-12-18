#pragma once
#ifndef ZEST_LIB_ENCODING_H
#define ZEST_LIB_ENCODING_H

#include <cstdint>

namespace Zest { namespace Lib {

struct Encoding
{
	using BYTE = uint8_t;
	enum class Type {
		ASCII,
		UTF8,
		UTF16BE,
		UTF16LE,
		UTF32BE,
		UTF32LE
	};
};


}}

#endif