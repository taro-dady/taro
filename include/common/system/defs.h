
#pragma once

#include "base/base.h"

#define NAMESPACE_TARO_SYSTEM_BEGIN namespace taro { namespace system {
#define NAMESPACE_TARO_SYSTEM_END } }

#define USING_NAMESPACE_TARO_SYSTEM using namespace taro::system;

NAMESPACE_TARO_SYSTEM_BEGIN

enum exception
{
    exception_data_invalid,
};

NAMESPACE_TARO_SYSTEM_END
