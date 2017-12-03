#pragma once
#ifndef ZEST_LIB_ERROR_H
#define ZEST_LIB_ERROR_H

#include <cstdint>
#include <string>
#include <memory>

namespace Zest { namespace Lib { namespace Error {

enum class ErrorCode : uint32_t;

struct IError
{
	virtual ErrorCode GetErrorCode() const = 0;
	virtual const char* GetErrorName() const = 0;
};

// Define ErrorCode here
#define ZEST_DEFINE_ERRORCODE_LIST(defineError) \
	defineError(TemplateError)\
	defineError(UnexpectedError)\
	defineError(AcessDeniedError)\
	defineError(UnexpectOperationError)\
	defineError(CannotCompareError)\
	defineError(MalFormatError)\

#define ZEST_DEFINE_ERRORCODE_VALUE(value) value##,
#define ZEST_DEFINE_DEFINE_ERRORCODE(List) enum class ErrorCode : uint32_t { List(ZEST_DEFINE_ERRORCODE_VALUE) }
/*
	enum class ErrorCode: uint32_t
	{
		TemplateError,
		...
	}
*/
ZEST_DEFINE_DEFINE_ERRORCODE(ZEST_DEFINE_ERRORCODE_LIST);

// Define Error object
#define STR_VALUE(value) #value

#define ZEST_DEFINE_DEFINE_ERROR(value) \
class value: public IError \
{ \
public:\
	ErrorCode GetErrorCode() const override\
	{\
		return ErrorCode::##value##;\
	}\
	const char* GetErrorName() const override\
	{\
		return m_cErrorName;\
	}\
private:\
static constexpr const char* m_cErrorName{ STR_VALUE(value) };\
};

ZEST_DEFINE_ERRORCODE_LIST(ZEST_DEFINE_DEFINE_ERROR)

// Define Error Maker
#define ErrorMake_CASE(value) \
case ErrorCode::##value##:\
{\
	return std::make_unique<##value##>();\
}

#define ZEST_DEFINE_DEFINE_ERRORMAKER(List) inline std::unique_ptr<IError> MakeError(ErrorCode errorCode) noexcept\
{\
	switch(errorCode)\
	{\
		List(ErrorMake_CASE)\
		default:\
			return nullptr;\
	}\
}

ZEST_DEFINE_DEFINE_ERRORMAKER(ZEST_DEFINE_ERRORCODE_LIST)

// Throw exception with error
class Exception : public std::exception
{
public:
	explicit Exception(ErrorCode errorCode) noexcept
		: m_errorCode{ std::move(MakeError(errorCode)) }
	{
	}

	virtual const char* what() const throw()
	{
		return m_errorCode->GetErrorName();
	}

private:
	std::unique_ptr<IError> m_errorCode;
};

#define DEFINE_THROWERROR(value) \
[[noreturn]] inline void Throw##value##Exception() \
{\
	throw Exception{ ErrorCode::##value };\
}

ZEST_DEFINE_ERRORCODE_LIST(DEFINE_THROWERROR)

}}}

#endif
