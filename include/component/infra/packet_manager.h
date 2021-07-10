
#pragma once

#include "infra/defs.h"
#include "function/inc.h"
#include "memory/packet.h"
#include <memory>

NAMESPACE_TARO_INFRA_BEGIN

// 包内存管理
class packet_allocator
{
public: // 公共函数

    /*
    * @brief 析构函数
    */
    virtual ~packet_allocator() = default;

    /*
    * @brief 申请内存
    *
    * @param[in]  bytes 内存大小
    * @return     内存指针
    */
    virtual void* alloc( uint32_t bytes ) = 0;

    /*
    * @brief 重新申请内存
    *
    * @param[in]  old_ptr 原有内存指针
    * @param[in]  bytes   内存大小
    * @return     新内存指针
    */
    virtual void* realloc( void* old_ptr, uint32_t bytes ) = 0;

    /*
    * @brief 释放内存
    *
    * @param[in]  mem_ptr 内存指针
    * @return     true 成功 false 失败
    */
    virtual bool release( void* mem_ptr ) = 0;
};

using packet_allocator_ptr = std::shared_ptr< packet_allocator >;

// 析构附加数据区
using destruct_extra_func = func::function< void( uint8_t*, uint32_t const& ) >;

// 包内存管理类
class packet_manager
{
public: // 公共函数 

    /*
    * @brief 获取单例
    */
    static packet_manager& instance();
    
    /*
    * @brief 设置自定义内存管理对象
    */
    void set_allocator( packet_allocator_ptr const& allocator );

private: // 私有函数
    
    /*
    * @brief 构造函数
    */
    packet_manager();

    /*
    * @brief 析构函数
    */
    ~packet_manager() = default;

    // 禁止拷贝构造
    packet_manager( packet_manager const& );
    packet_manager& operator=( packet_manager const& );
};

/*
* @brief      创建数据包
*
* @param[in]  cap        容量
* @param[in]  extra_size 附加数据大小
* @param[in]  power      扩容时最小块大小 2^power
* @param[in]  func       附加数据区析构函数( 附加数据区有需要手动释放的变量时使用 )
* @return     动态数据指针
*/
extern memory::packet_ptr create_default_packet( uint32_t cap                    = 0,
                                                 uint32_t extra_size             = 0,
                                                 uint32_t power                  = 8,
                                                 destruct_extra_func const& func = destruct_extra_func() );

/*
* @brief      创建数据包的引用(内容不变，仅仅将字节计数进行拷贝，用于多处引用同一个数据的不同部分)
*
* @param[in]  in  被引用对象
* @return     动态数据指针
*/
memory::packet_ptr create_default_ref_packet( memory::packet_ptr const& in );

NAMESPACE_TARO_INFRA_END
