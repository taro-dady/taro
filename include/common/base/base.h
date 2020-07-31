
#pragma once

#include "base/type.h"

#ifndef  __UNIT_TEST__
    #define PUBLIC    public
    #define PROTECTED protected
    #define PRIVATE   private
#else
    #define PUBLIC    public
    #define PROTECTED public
    #define PRIVATE   public
#endif

#define NAMESPACE_TARO_BEGIN namespace taro {
#define NAMESPACE_TARO_END }

#define USING_NAMESPACE_TARO using namespace taro;
