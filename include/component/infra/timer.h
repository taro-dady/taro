
#pragma once

#include "infra/defs.h"
#include "function/inc.h"
#include <vector>

NAMESPACE_TARO_INFRA_BEGIN

// 定时器内部结构
struct timer_impl;

// 定时器对象
class timer
{
public: // 公共类型定义

    /*
    * @brief  定时器回调
    */
    using timer_cb_t = func::functor< void >;

public: // 公共函数定义

    /*
    * @brief: 构造函数
    *
    * @param[in]  name  定时器名称
    */
    timer( const char* name );

    /*
    * @brief      析构函数
    */
    ~timer();

    /*
    * @brief 禁止拷贝构造
    */
    timer( timer const& ) = delete;

    /*
    * @brief 禁止赋值
    */
    timer& operator=( timer const& ) = delete;

    /*
    * @brief      启动定时器
    *
    * @param[in]  delay  延时启动时间
    * @param[in]  period 定时器周期
    * @param[in]  cb     定时器到时回调
    */
    int32_t start( uint32_t delay, uint32_t period, timer_cb_t cb );

    /*
    * @brief      停止定时器
    * 
    * @param[in]  wait 是否等待回调完成
    */
    int32_t stop( bool wait = true );

    /*
    * @brief      修改时间
    *
    * @param[in]  period 定时器周期
    */
    int32_t modify( uint32_t period );

    /*
    * @brief      是否正在运行
    * 
    * @return     true 运行中 false 已停止
    */
    bool is_running() const;

private: // 私有变量定义

    std::shared_ptr<timer_impl> _impl;
};

// 定时器信息
struct timer_info
{
    std::string name;     // 定时器名称
    bool        running;  // 定时器是否运行
    int64_t     cost;     // 定时器运行时间 running = true有效
};

/*
* @brief      启动定时管理对象
*
* @param[in]  worker_num 处理线程数量 0表示不使用异步线程池，>0表示使用异步线程的数量
* @return     errno_e
*/
extern int32_t start_timer_manager( uint32_t worker_num = 4 );

/*
* @brief      停止定时器
* 
* @return     errno_e
*/
extern int32_t stop_timer_manager();

/*
* @brief      获取定时器对象信息
*
* @param[out] info 定时器信息
* @return     errno_e
*/
extern int32_t get_timer_info( std::vector<timer_info>& info );

NAMESPACE_TARO_INFRA_END
