#pragma once
#ifndef ZEST_LIB_STREAM_H
#define ZEST_LIB_STREAM_H

#include <vector>
#include "Encoding.h"

namespace Zest { namespace Lib {

#define DEFAULT_READ_SIZE 4096

struct IStream
{
	virtual std::vector<Encoding::BYTE> Read(size_t readSize = DEFAULT_READ_SIZE) = 0;
	virtual Encoding::BYTE Peek() = 0;
	virtual void Seek(size_t seekSize) = 0;

};

class CodePoint
{

};

}}

#endif