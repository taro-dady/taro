
#pragma once

#include "function/defs.h"
#include <tuple>

NAMESPACE_TARO_FUNCTION_BEGIN

template<class T>
struct type {};

template<class T, int32_t N = std::tuple_size<T>::value >
struct do_tuple_functor
{
    template<typename R, class F, class ...Args>
    static R call( type<R>, F&& f, T&& t, Args&&... args )
    {
        return do_tuple_functor<T, N - 1>::call( type<R>(), 
                                                 std::forward<F>( f ), 
                                                 std::forward<T>( t ),
                                                 std::forward<typename std::tuple_element<N - 1, T>::type>( std::get<N - 1>( t ) ),
                                                 std::forward<Args>(args)... );
    }

    template<class F, class ...Args>
    static void call( type<void>, F&& f, T&& t, Args&&... args )
    {
        do_tuple_functor<T, N - 1>::call( type<void>(),
                                          std::forward<F>( f ),
                                          std::forward<T>( t ),
                                          std::forward<typename std::tuple_element<N - 1, T>::type>( std::get<N - 1>( t ) ),
                                          std::forward<Args>( args )... );
    }
};

template<class T >
struct do_tuple_functor<T, 0>
{
    template<typename R, class F, class ...Args>
    static R call( type<R>, F&& f, T&&, Args&&... args )
    {
        return f( args... );
    }

    template<class F, class ...Args>
    static void call( type<void>, F&& f, T&&, Args&&... args )
    {  
        f( args... );
    }
};

NAMESPACE_TARO_FUNCTION_END
