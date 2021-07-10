
#pragma once

#include "base/macro_defs.h"
#include "infra/reflection.h"

NAMESPACE_TARO_INFRA_BEGIN

template<typename C, typename T>
void class_variable_maker( std::vector< variable_reflection_base_ptr >& v, T&& t )
{
    v.emplace_back( std::make_shared< variable_reflection<C, T> >( std::forward<T>( t ) ) );
};

template<typename C, typename T, typename... Args>
void class_variable_maker( std::vector< variable_reflection_base_ptr >& v, T&& t, Args&&... args )
{
    v.emplace_back( std::make_shared< variable_reflection<C, T> >( std::forward<T>( t ) ) );
    class_variable_maker<C>( v, std::forward<Args>( args )... );
};

NAMESPACE_TARO_INFRA_END

#define MAKE_META_DATA( STRUCT_NAME, N, ... ) \
    struct _REFLECTION_##STRUCT_NAME##_TYPE_\
    {\
        _REFLECTION_##STRUCT_NAME##_TYPE_()\
        {\
            std::vector<std::string> member_vec = { MARCO_EXPAND( MACRO_CONCAT(CON_STR, N)(__VA_ARGS__) ) };\
            std::vector< taro::infra::variable_reflection_base_ptr > base_vec;\
            taro::infra::class_variable_maker<STRUCT_NAME>( base_vec, MAKE_ARG_LIST( N, &STRUCT_NAME::FIELD, __VA_ARGS__ ) );\
            taro::infra::reflection_manager::instance().register_class( #STRUCT_NAME, &typeid( STRUCT_NAME ) );\
            for( size_t index = 0; index < member_vec.size(); ++index )\
            {\
                base_vec[index]->set_name( member_vec[index].c_str() ); \
                taro::infra::reflection_manager::instance().register_class_member( #STRUCT_NAME, base_vec[index] ); \
            }\
        }\
    };\
    static _REFLECTION_##STRUCT_NAME##_TYPE_ _##STRUCT_NAME##_REG_INST_;

#define REFLECTION( STRUCT_NAME, ... ) \
    MAKE_META_DATA( STRUCT_NAME, GET_ARG_COUNT( __VA_ARGS__ ), __VA_ARGS__ )
