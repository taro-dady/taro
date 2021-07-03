
#pragma once

#include "function/functor.h"

NAMESPACE_TARO_FUNCTION_BEGIN

// 普通函数
template<typename F>
typename std::enable_if< !is_member_func< F >::value && !is_obj_func<F>::value, func::function< typename function_traits<F>::function_type > >::type
make_func( F const& f )
{
    return func::function< typename function_traits<F>::function_type >( f );
}

// 类函数
template<typename F, typename Class>
typename std::enable_if< std::is_member_function_pointer< F >::value, func::function< typename function_traits<F>::function_type > >::type
make_func( F const& f, Class const& obj )
{
    return func::function< typename function_traits<F>::function_type>( f, obj );
}

// 对象类函数
template<typename F>
typename std::enable_if< is_obj_func<F>::value, func::function< typename function_traits<F>::function_type > >::type
make_func( F const& f )
{
    return func::function< typename function_traits<F>::function_type >( f );
}

// 普通函数
template<typename F, typename... Args>
typename std::enable_if< !std::is_member_function_pointer<F>::value && !is_obj_func<F>::value, func::functor< typename std::result_of<F( Args... )>::type > >::type
bind( F const& f, Args&&... args )
{
    using R      = typename function_traits<F>::result_type;
    using func_t = typename func::function< typename function_traits<F>::function_type >;
    auto ptr     = std::make_shared< binder_functor<func_t, R, Args...> >( func_t( f ), std::forward<Args>( args )... );
    return functor< R >( std::dynamic_pointer_cast< functor_base< R > >( ptr ) );
}

// 类函数
template<typename F, typename Class, typename... Args>
typename std::enable_if< std::is_member_function_pointer< F >::value, func::functor< typename function_traits<F>::result_type > >::type
bind( F const& f, Class const& obj, Args&&... args )
{
    using R      = typename function_traits<F>::result_type;
    using func_t = typename func::function< typename function_traits<F>::function_type >;
    auto ptr     = std::make_shared< binder_functor<func_t, R, Args...> >( func_t( f, obj ), std::forward<Args>( args )... );
    return functor< R >( std::dynamic_pointer_cast< functor_base< R > >( ptr ) );
}

// 对象类函数
template<typename F, typename... Args>
typename std::enable_if< is_obj_func<F>::value, func::functor< typename std::result_of<F( Args... )>::type > >::type
bind( F const& f, Args&&... args )
{
    using R  = typename std::result_of<F( Args... )>::type;
    auto ptr = std::make_shared< binder_functor<F, R, Args...> >( f, std::forward<Args>( args )... );
    return functor<R>( std::dynamic_pointer_cast< functor_base<R> >( ptr ) );
}

// 调用function会产生异常，该函数用于处理函数异常
template< typename F, typename... Args >
result < typename std::enable_if< std::is_same< void, typename F::result_t >::value >::type >
function_call( F f, Args... args )
{
    result<void> ret;
    try
    {
        f( args... );
    }
    catch( func::exception )
    {
        ret.set_errno( errno_invalid_res );
    }
    return ret;
}

// 调用function会产生异常，该函数用于处理函数异常
template< typename F, typename... Args >
result <  typename std::enable_if< !std::is_same< void, typename F::result_t >::value, typename F::result_t >::type >
function_call( F f, Args... args )
{
    using result_t = typename F::result_t;

    result<result_t> ret;
    try
    {
        ret.set_value( f( args... ) );
    }
    catch( func::exception )
    {
        ret.set_errno( errno_invalid_res );
    }
    return ret;
}

NAMESPACE_TARO_FUNCTION_END
