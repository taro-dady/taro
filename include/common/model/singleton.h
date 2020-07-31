
#pragma once

#include "model/defs.h"

NAMESPACE_TARO_MODEL_BEGIN

template<class T>
class singleton
{
public:  // 公共函数定义

    /*
    * @brief: 获取单例
    */
    static T& instance()
    {
        static T inst;
        return inst;
    }
};

NAMESPACE_TARO_MODEL_END
