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
#define ERRORCODE_LIST(defineError) \
	defineError(TemplateError)\
	defineError(UnexpectedError)\
	defineError(AcessDeniedError)\
	defineError(UnexpectOperationError)\

#define ERRORCODE_VALUE(value) value##,
#define DEFINE_ERRORCODE(List) enum class ErrorCode : uint32_t { List(ERRORCODE_VALUE) }
/*
	enum class ErrorCode: uint32_t
	{
		TemplateError,
		...
	}
*/
DEFINE_ERRORCODE(ERRORCODE_LIST);

// Define Error object
#define STR_VALUE(value) #value

#define DEFINE_ERROR(value) \
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

ERRORCODE_LIST(DEFINE_ERROR)

// Define Error Maker
#define ErrorMake_CASE(value) \
case ErrorCode::##value##:\
{\
	return std::make_unique<##value##>();\
}

#define DEFINE_ERRORMAKER(List) std::unique_ptr<IError> MakeError(ErrorCode errorCode) noexcept\
{\
	switch(errorCode)\
	{\
		List(ErrorMake_CASE)\
		default:\
			return nullptr;\
	}\
}

DEFINE_ERRORMAKER(ERRORCODE_LIST)

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
void Throw##value##Exception() \
{\
	throw Exception{ ErrorCode::##value };\
}

ERRORCODE_LIST(DEFINE_THROWERROR)

}}}

#endif
