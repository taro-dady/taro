
#pragma once

#include "infra/defs.h"
#include "memory/scoped_ptr.h"

NAMESPACE_TARO_INFRA_BEGIN

class dyn_lib
{
public: // 公共函数

    /*
    * @brief      构造函数
    */
    dyn_lib();

    /*
    * @brief      析构函数
    */
    ~dyn_lib();

    /*
    * @brief      加载动态库
    *
    * @param[in]  name 动态库名称
    */
    int32_t load( const char* name );

    /*
    * @brief      卸载动态库
    */
    int32_t unload();

    /*
    * @brief      获取函数地址
    *
    * @param[in]  func_name 函数名称
    * @return     void*
    */
    void* func_addr( const char* func_name );

private: // 私有变量

    struct impl;
    memory::scoped_ptr<impl> _impl;
};

NAMESPACE_TARO_INFRA_END
