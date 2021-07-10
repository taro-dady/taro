
#pragma once

#include "infra/timer.h"
#include <mutex>
#include <atomic>
#include <string>

NAMESPACE_TARO_INFRA_BEGIN

// 定时器内部结构
struct timer_impl : PUBLIC std::enable_shared_from_this<timer_impl>
{
PUBLIC: // 公共函数定义

    /*
    * @brief: 构造函数
    *
    * @param[in]  name     定时器名称
    */
    timer_impl( const char* name );

    /*
    * @brief      析构函数
    */
    ~timer_impl();

    /*
    * @brief      获取标识
    */
    uint32_t identity() const;

    /*
    * @brief      启动定时器
    *
    * @param[in]  delay  延时启动时间
    * @param[in]  period 定时器周期
    * @param[in]  cb     定时器到时回调
    * @return     TARO_errno_e
    */
    int32_t start( uint32_t delay, uint32_t period, timer::timer_cb_t cb );

    /*
    * @brief      停止定时器
    * 
    * @param[in]  wait 是否等待回调完成
    * @return     TARO_errno_e
    */
    int32_t stop( bool wait = true );

    /*
    * @brief      修改时间
    *
    * @param[in]  period 定时器周期
    * @return     TARO_errno_e
    */
    int32_t modify( uint32_t period );

    /*
    * @brief      是否正在运行
    * 
    * @return     true 运行中 false 已停止
    */
    bool is_running() const;

    /*
    * @brief      业务处理
    */
    void invoke();
    
    /*
    * @brief      更新调用时间
    */
    bool update_call_time();

    /*
    * @brief      获取调用时间
    */
    int64_t get_call_time() const;

    /*
    * @brief      获取定时器信息
    */
    timer_info get_info() const;

PRIVATE: // 私有变量定义

    bool                 _started;    // 启动标识
    int64_t              _call_tm;    // 下一次调用时间
    uint32_t             _identity;   // 定时器标识，用于唯一标识该定时器
    uint32_t             _period;     // 周期
    std::string          _name;       // 名称
    std::atomic_bool     _running;    // 运行标识
    timer::timer_cb_t    _timer_cb;   // 到时回调
    mutable std::mutex   _mutex;
    std::atomic<int64_t> _run_tp;     // 调用回调的起始时间，_running为true时有效
};

using timer_impl_ptr = std::shared_ptr< timer_impl >;

// 定时器比较对象
inline bool timer_compare( timer_impl_ptr const& l, timer_impl_ptr const& r )
{
    return l->get_call_time() < r->get_call_time();
}

NAMESPACE_TARO_INFRA_END
