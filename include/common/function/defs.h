
#pragma once

#include "base/inc.h"

#define NAMESPACE_TARO_FUNCTION_BEGIN namespace taro { namespace func {
#define NAMESPACE_TARO_FUNCTION_END } }
#define USING_NAMESPACE_TARO_FUNCTION using namespace taro::func;

NAMESPACE_TARO_FUNCTION_BEGIN

enum exception
{
    exception_functor_invalid,
    exception_function_invalid,
    exception_weak_pointer_expired,
};

NAMESPACE_TARO_FUNCTION_END
