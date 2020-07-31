
#pragma once

#include <functional>
#include "function/function.h"

template <typename T>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<R( Args... )>
{
    using result_type = R;

    using function_type = R( Args... );

    enum { arg_num = sizeof...( Args ) };

    template <size_t I>
    struct arg
    {
        typedef typename std::tuple_element< I, std::tuple<Args...> >::type type;
    };
};

template <typename T>
struct function_traits : public function_traits<decltype( &T::operator() )>{};

template <typename R, typename... Args>
struct function_traits<R(*)( Args... )> : public function_traits<R( Args... )>{};

template <typename F>
struct function_traits< std::function<F> > : public function_traits<F>{};

template <typename F>
struct function_traits< taro::func::function<F> > : public function_traits<F>{};

//member function
#define FUNCTION_TRAITS(...) \
    template <typename R, typename C, typename... Args>\
    struct function_traits<R(C::*)(Args...) __VA_ARGS__> : function_traits<R(Args...)>{}; \

FUNCTION_TRAITS()
FUNCTION_TRAITS( const )
FUNCTION_TRAITS( volatile )
FUNCTION_TRAITS( const volatile )

template<typename T>
struct is_member_func
{
    static constexpr bool value = false;
};

#define IS_MEMBER_TRAITS(...) \
    template <typename R, typename C, typename... Args>\
    struct is_member_func<R(C::*)(Args...) __VA_ARGS__>{ static constexpr bool value = true; }; \

IS_MEMBER_TRAITS()
IS_MEMBER_TRAITS( const )
IS_MEMBER_TRAITS( volatile )
IS_MEMBER_TRAITS( const volatile )

template<typename T>
struct is_obj_func
{
    static constexpr bool value = true;
};

template <typename R, typename... Args>
struct is_obj_func<R( * )( Args... )>
{
    static constexpr bool value = false;
};

template <typename R, typename... Args>
struct is_obj_func<R( Args... )>
{
    static constexpr bool value = false;
};

#define IS_OBJ_TRAITS(...) \
    template <typename R, typename C, typename... Args>\
    struct is_obj_func<R(C::*)(Args...) __VA_ARGS__>{ static constexpr bool value = false; }; \

IS_OBJ_TRAITS()
IS_OBJ_TRAITS( const )
IS_OBJ_TRAITS( volatile )
IS_OBJ_TRAITS( const volatile )
