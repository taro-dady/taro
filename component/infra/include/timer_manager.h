
#pragma once

#include "timer_impl.h"
#include "model/singleton.h"
#include "system/thread_pool.h"
#include <list>
#include <mutex>
#include <condition_variable>

NAMESPACE_TARO_INFRA_BEGIN

// 定时器管理对象
class timer_manager
{
PUBLIC: // 公共函数定义

    /*
    * @brief      构造函数
    */
    timer_manager();

    /*
    * @brief      析构函数
    */
    ~timer_manager();

    /*
    * @brief      启动定时管理对象
    *
    * @param[in]  worker_num 处理线程数量 0表示不使用异步线程池，>0表示使用异步线程的数量
    * @return     errno_e
    */
    int32_t start( uint32_t worker_num );

    /*
    * @brief      停止定时器
    * 
    * @return     errno_e
    */
    int32_t stop();

    /*
    * @brief      添加定时器
    *
    * @param[in]] timer 定时器对象
    * @return     errno_e
    */
    int32_t add_timer( timer_impl_ptr const& timer_ptr );

    /*
    * @brief      删除定时器
    *
    * @param[in]] timer 定时器对象
    * @return     errno_e
    */
    int32_t remove_timer( timer_impl_ptr const& timer_ptr );

    /*
    * @brief      更新定时器排序
    */
    void update_timer();

    /*
    * @brief      获取定时器对象信息
    *
    * @param[out] timer_info 定时器信息
    * @return     errno_e
    */
    int32_t get_timer_info( std::vector<timer_info>& info ) const;

PRIVATE: // 私有类型定义

    using timer_container_t   = std::list< timer_impl_ptr >;
    using timer_thrd_pool_t   = system::kthread_pool< uint32_t, func::functor<void> >;
    using timer_thrd_pool_ptr = std::unique_ptr< timer_thrd_pool_t >;

PRIVATE: // 私有函数定义

    /*
    * @brief  线程函数
    */
    void thrd_proc();

    /*
    * @brief  唤醒
    */
    void modify( int64_t delay );

    /*
    * @brief  定时器被唤醒
    */
    void invoke();

PRIVATE: // 私有变量定义

    bool                    _looping;
    bool                    _modified;
    int64_t                 _wakeup_tm;
    std::thread             _thrd;
    timer_container_t       _container;
    mutable std::mutex      _mutex;
    timer_thrd_pool_ptr     _thrd_pool;
    std::condition_variable _cond;
};

#define timer_manager_inst model::singleton< timer_manager >::instance()

NAMESPACE_TARO_INFRA_END
