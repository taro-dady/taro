
#pragma once

#include "basic/defs.h"

NAMESPACE_TARO_BASIC_BEGIN

/**
* @brief      获取当前线程号
*/
extern uint64_t current_tid();

/**
* @brief      线程睡眠 单位(毫秒)
*/
extern void thrd_sleep( uint32_t ms );

NAMESPACE_TARO_BASIC_END
