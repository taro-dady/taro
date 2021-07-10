
#pragma once

#include "infra/packet_manager.h"

NAMESPACE_TARO_INFRA_BEGIN

// 默认内存管理器
class default_allocator : PUBLIC packet_allocator
{
    /*
    * @brief 申请内存
    *
    * @param[in]  bytes 内存大小
    * @return     内存指针
    */
    virtual void* alloc( uint32_t bytes ) override final;

    /*
    * @brief 重新申请内存
    *
    * @param[in]  old_ptr 原有内存指针
    * @param[in]  bytes   内存大小
    * @return     新内存指针
    */
    virtual void* realloc( void* old_ptr, uint32_t bytes ) override final;

    /*
    * @brief 释放内存
    *
    * @param[in]  mem_ptr 内存指针
    * @return     true 成功 false 失败
    */
    virtual bool release( void* mem_ptr ) override final;
};

NAMESPACE_TARO_INFRA_END
