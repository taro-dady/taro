
#pragma once

// 公共错误码
enum ret_code_e
{
    errno_ok            =  0,
    errno_general       = -1,
    errno_invalid_param = -2,
    errno_memory_out    = -3,
    errno_unaccess      = -4,
    errno_disconnect    = -5,
    errno_unspported    = -6,
    errno_invalid_res   = -7,
};
