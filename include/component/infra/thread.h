
#pragma once

#include "infra/defs.h"
#include "function/inc.h"
#include "memory/scoped_ptr.h"
#include <vector>
#include <string>

NAMESPACE_TARO_INFRA_BEGIN

// 线程内部变量
class thread_impl;

// 线程对象
class thread 
{
public: // 公共类型

    // 线程优先级
    enum thread_priority_e
    {
        thread_prio_low     = 0,
        thread_prio_middle  = 1,
        thread_prio_high    = 2,
    };

    // 线程回调函数
    using thread_cb_t = func::functor< void >;

public: // 公共定义

    /*
    * @brief      构造函数
    *
    * @param[in]  name 线程名称(名称不能重复，否则在启动时会失败)
    */
    thread( const char* name );

    /*
    * @brief      析构函数
    */
    ~thread();

    /*
    * @brief      启动线程
    * 
    * @param[in]  cb 线程回调函数
    * @param[in]  stack_size 线程栈大小  =0 使用默认栈大小
    * @param[in]  priority   线程优先级
    * @retval     参考 toft_errno_e
    */
    int32_t start( thread_cb_t const& cb, uint32_t stack_size = 0, thread_priority_e priority = thread_prio_low );

    /*
    * @brief      停止线程
    * 
    * @retval     参考 toft_errno_e
    */
    int32_t stop();

    /*
    * @brief      获取当前线程的线程号
    * 
    * @retval     > 0 线程ID <=0 参考 toft_errno_e
    */
    static uint64_t current_tid();

    /*
    * @brief      线程休眠
    *
    * @param[in]  ms 休眠时间 > 0 休眠毫秒数 = 0 让出线程
    */
    static void sleep( uint32_t ms );

private: // 私有变量

    memory::scoped_ptr< thread_impl > _impl;
};

// 线程信息对象
struct thread_info_t
{
    std::string               name;        // 线程名称
    bool                      is_running;  // 线程是否正在运行
    uint32_t                  stack_size;  // 栈大小
    int64_t                   run_time;    // 线程运行时间毫秒
    thread::thread_priority_e priority;    // 优先级
};

/*
* @brief      获取所有线程信息
* 
* @retval     所有线程信息
*/
extern std::vector< thread_info_t > get_all_thread_info();

NAMESPACE_TARO_INFRA_END
