
#pragma once

#include "basic/defs.h"
#include "memory/packet.h"
#include "function/function.h"

NAMESPACE_TARO_BASIC_BEGIN

// 析构附加数据区
using destruct_extra_func = func::function< void( uint8_t*, uint32_t const& ) >;

/**
* @brief      创建默数据包
*
* @param[in]  cap        容量
* @param[in]  extra_size 附加数据大小
* @param[in]  power      扩容时最小块大小 2^power
* @param[in]  func       附加数据区析构函数( 附加数据区有需要手动释放的变量时使用 )
* @return     动态数据指针
*/
memory::packet_ptr create_packet( uint32_t cap = 0, uint32_t extra_size = 0, uint32_t power = 8, destruct_extra_func const& func = destruct_extra_func() );

NAMESPACE_TARO_BASIC_END
