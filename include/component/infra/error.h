
#pragma once

#include "infra/defs.h"

NAMESPACE_TARO_INFRA_BEGIN

/*
* @brief  设置错误号
*/
void set_err_no( int32_t err_no );

/*
* @brief  获取错误号
*/
int32_t get_err_no( void );

/*
* @brief  设置错误信息
*/
void set_err_msg( const char* msg );

/*
* @brief  获取错误信息
*
* @return     const char*
* @retval     null 没有错误信息 other 错误信息
*/
const char* get_err_msg( void );

NAMESPACE_TARO_INFRA_END
