
#include "infra/file_sys.h"
#include "infra/error.h"
#include "utils/defer.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <sys/statfs.h>

NAMESPACE_TARO_INFRA_BEGIN

file_sys& file_sys::instance()
{
    static file_sys inst;
    return inst;
}

file_sys::file_sys()
{

}

file_sys::~file_sys()
{

}

std::list<file_sys::fsys_node> file_sys::get_nodes( const char* dir, uint32_t cond )
{
    std::list<file_sys::fsys_node> ret;

    if( nullptr == dir || 0 == strlen( dir ) )
    {
        set_err_no( errno_invalid_param );
        return ret;
    }

    struct stat s;
    lstat( dir, &s );
    if( !S_ISDIR( s.st_mode ) )
    {
        set_err_no( errno_unaccess );
        return ret;
    }

    DIR* dir_obj = opendir( dir );
    if( nullptr == dir_obj )
    {
        set_err_no( errno_unaccess );
        return ret;
    }

    utils::defer rc( [dir_obj]()
    { 
        closedir( dir_obj ); 
    } );

    struct dirent* filename;
    while( ( filename = readdir( dir_obj ) ) != nullptr )
    {
        if( 0 == strcmp( filename->d_name, "."  ) ||
            0 == strcmp( filename->d_name, ".." ) )
        {
            continue;
        }
        
        std::string full_path = dir;
        full_path += "/";
        full_path += filename->d_name;
        lstat( full_path.c_str(), &s );

        fsys_node node;
        node.name  = filename->d_name;
        node.mtime = s.st_mtime;
        node.size  = s.st_size;

        if( S_ISDIR( s.st_mode ) )
        {
            node.type = node_type_dir;
        }
        else if( S_ISREG( s.st_mode ) )
        {
            node.type = node_type_file;
        }
        else
        {
            continue;
        }
        ret.emplace_back( node );
    }
    return ret;
}

int32_t file_sys::get_disk_info( const char* dir, file_sys::disk_info& info )
{
    if( nullptr == dir || 0 == strlen( dir ) )
    {
        return errno_invalid_param;
    }

    struct statfs disk_info;
    if( statfs( dir, &disk_info ) < 0 )
    {
        return errno_unaccess;
    }

    unsigned long long block_size = disk_info.f_bsize;
    unsigned long long total_size = block_size * disk_info.f_blocks;
    unsigned long long free_disk  = disk_info.f_bfree * block_size;

    info.occupy_percent = ( ( double )free_disk / ( double )total_size ) * 100;
    info.occupy_percent = 100 - info.occupy_percent;
    info.free_bytes     = free_disk;
    info.total_bytes    = total_size;
    return errno_ok;
}

int32_t file_sys::is_dir_exist( const char* dir )
{
    if( nullptr == dir || 0 == strlen( dir ) )
    {
        return errno_invalid_param;
    }

    DIR *dp;
    if( nullptr == ( dp = opendir( dir ) ) )
    {
        return errno_unaccess;
    }

    closedir( dp );
    return errno_ok;
}

int32_t file_sys::create_dir( const char* dir )
{
    if( nullptr == dir || 0 == strlen( dir ) )
    {
        return errno_invalid_param;
    }

    return ( 0 == ::mkdir( dir, 0755 ) ) ? errno_ok : errno_unaccess;
}

int32_t file_sys::remove_dir( const char* dir )
{
    if( nullptr == dir || 0 == strlen( dir ) )
    {
        return errno_invalid_param;
    }

    std::string cmd = "rm -rf ";
    cmd += dir;
    return ( 0 == system( cmd.c_str() ) ) ? errno_ok : errno_unaccess;
}

int32_t file_sys::copy_dir( const char* src, const char* dst )
{
    if( nullptr == src || 0 == strlen( src )
     || nullptr == dst || 0 == strlen( dst ) )
    {
        return errno_invalid_param;
    }

    if( errno_ok != is_dir_exist( src ) )
    {
        printf("11\n");
        return errno_unaccess;
    }

    if( errno_ok != is_dir_exist( dst ) )
    {
        if( errno_ok != create_dir( dst ) )
        {
            printf("22\n");
            return errno_unaccess;
        }
    }

    std::string cmd = "cp -rf ";
    cmd += src;
    cmd += "/* ";
    cmd += dst;

    return ( 0 == system( cmd.c_str() ) ) ? errno_ok : errno_general;
}

int32_t file_sys::is_file_exist( const char* file )
{
    if( nullptr == file || 0 == strlen( file ) )
    {
        return errno_invalid_param;
    }

    struct stat st;
    if( 0 != stat( file, &st ) )
    {
        return errno_unaccess;
    }
    return S_ISREG( st.st_mode ) ? errno_ok : errno_unaccess;
}

int32_t file_sys::get_file_size( const char* file, uint64_t& bytes )
{
    if( nullptr == file || strlen( file ) == 0 )
    {
        return errno_invalid_param;
    }

    struct stat st;
    if( 0 != stat( file, &st ) )
    {
        return errno_unaccess;
    }

    if( S_ISREG( st.st_mode ) )
    {
        bytes = st.st_size;
        return errno_ok;
    }
    return errno_general;
}

int32_t file_sys::copy_file( const char* src, const char* dst )
{
    if( nullptr == src || strlen( src ) == 0
     || nullptr == dst || strlen( dst ) == 0 )
    {
        return errno_invalid_param;
    }

    std::string cmd = "cp -rf ";
    cmd += src;
    cmd += " ";
    cmd += dst;

    return ( 0 == system( cmd.c_str() ) ) ? errno_ok : errno_unaccess;
}


int32_t file_sys::remove_file( const char* path )
{
    if( nullptr == path || strlen( path ) == 0 )
    {
        return errno_invalid_param;
    }

    return ( 0 == ::remove( path ) ) ? errno_ok : errno_general;
}

NAMESPACE_TARO_INFRA_END
