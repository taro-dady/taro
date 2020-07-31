
#include "basic/thread.h"
#include <thread>
#include <windows.h>

NAMESPACE_TARO_BASIC_BEGIN

extern uint64_t current_tid()
{
    return ( uint64_t )GetCurrentThreadId();
}

extern void thrd_sleep( uint32_t ms )
{
    std::this_thread::sleep_for( std::chrono::milliseconds( ms ) );
}

NAMESPACE_TARO_BASIC_END
