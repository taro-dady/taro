
#pragma once

#include "basic/defs.h"
#include "memory/scoped_ptr.h"

NAMESPACE_TARO_BASIC_BEGIN

class dyn_lib
{
public: // 公共函数

    /**
    * @brief      构造函数
    */
    dyn_lib();

    /**
    * @brief      加载动态库
    *
    * @param[in]  name 动态库名称
    */
    ret_val load( const char* name );

    /**
    * @brief      卸载动态库
    */
    ret_val unload();

    /**
    * @brief      获取函数地址
    *
    * @param[in]  func_name 函数名称
    * @return     void*
    */
    result<void*> func_addr( const char* func_name );

private: // 私有变量

    struct impl;
    memory::scoped_ptr<impl> _impl;
};

NAMESPACE_TARO_BASIC_END
