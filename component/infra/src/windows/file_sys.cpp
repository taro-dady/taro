
#include "infra/error.h"
#include "infra/file_sys.h"
#include <direct.h>
#include <windows.h>
#include <shellapi.h>

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

    HANDLE          hFind;
    WIN32_FIND_DATA findData;

    std::string dir_new = dir;
    dir_new += "/*.*";
    hFind = FindFirstFile( dir_new.c_str(), &findData );    // 查找目录中的第一个文件
    if( hFind == INVALID_HANDLE_VALUE )
    {
        set_err_no( errno_unaccess );
        return ret;
    }

    do
    {
        if( 0 == strcmp( findData.cFileName, "." ) || 0 == strcmp( findData.cFileName, ".." ) )
        {
            continue;
        }

        if(  
             (  ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) && !( cond & node_type_dir  ) ) 
          || ( !( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) && !( cond & node_type_file ) )
          )
        {
            continue;
        }

        fsys_node node;
        node.name  = findData.cFileName;
        node.mtime = ( ( uint64_t )findData.ftLastWriteTime.dwHighDateTime << 32 ) | findData.ftLastWriteTime.dwLowDateTime;
        node.size  = ( ( uint64_t )findData.nFileSizeHigh << 32 ) | findData.nFileSizeLow;
        node.type  = ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ? node_type_dir : node_type_file;
        ret.emplace_back( node );

    } while( FindNextFile( hFind, &findData ) );    // 查找目录中的下一个文件

    FindClose( hFind );    // 关闭搜索句柄
    return ret;
}

int32_t file_sys::get_disk_info( const char* dir, file_sys::disk_info& info )
{
    if( nullptr == dir || 0 == strlen( dir ) )
    {
        return errno_invalid_param;
    }

    DWORD64 free_bytes           = 0;
    DWORD64 total_bytes          = 0;
    DWORD64 free_bytes_to_caller = 0;

    BOOL bResult = GetDiskFreeSpaceEx( TEXT( dir ), ( PULARGE_INTEGER )&free_bytes_to_caller, ( PULARGE_INTEGER )&total_bytes, ( PULARGE_INTEGER )&free_bytes );
    if( !bResult )
    {
        return errno_unaccess;
    }
    
    info.occupy_percent = ( uint32_t )( ( ( double )free_bytes / ( double )total_bytes ) * 100 );
    info.occupy_percent = 100 - info.occupy_percent;
    info.free_bytes     = free_bytes;
    info.total_bytes    = total_bytes;
    return errno_ok;
}

int32_t file_sys::is_dir_exist( const char* dir )
{
    if( nullptr == dir || 0 == strlen( dir ) )
    {
        set_err_no( errno_invalid_param );
        return false;
    }

    WIN32_FIND_DATA  FindFileData;
    BOOL bValue  = false;
    HANDLE hFind = FindFirstFile( dir, &FindFileData );
    if( ( hFind != INVALID_HANDLE_VALUE ) && ( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
    {
        bValue = TRUE;
    }

    if( hFind != INVALID_HANDLE_VALUE )
    {
        FindClose( hFind );
    }
    return TRUE == bValue ? errno_ok : errno_general;
}

int32_t file_sys::create_dir( const char* dir )
{
    if( nullptr == dir || 0 == strlen( dir ) )
    {
        return errno_invalid_param;
    }
    return ( 0 == _mkdir( dir ) ) ? errno_ok : errno_unaccess;
}

int32_t file_sys::remove_dir( const char* dir )
{
    if( nullptr == dir || 0 == strlen( dir ) )
    {
        return errno_invalid_param;
    }

    SHFILEOPSTRUCT fop;
    ZeroMemory( ( void* )&fop, sizeof( SHFILEOPSTRUCT ) );
    fop.wFunc  = FO_DELETE;
    fop.pFrom  = dir;
    fop.pTo    = NULL;
    fop.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;
    
    return ( 0 == SHFileOperation( &fop ) ) ? errno_ok : errno_unaccess;
}

int32_t file_sys::copy_dir( const char* src, const char* dst )
{
    if( nullptr == src || 0 == strlen( src )
     || nullptr == dst || 0 == strlen( dst ) )
    {
        return errno_invalid_param;
    }

    SHFILEOPSTRUCT fop;
    ZeroMemory( ( void* )&fop, sizeof( SHFILEOPSTRUCT ) );
    fop.wFunc  = FO_COPY;
    fop.pFrom  = src;
    fop.pTo    = dst;
    fop.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;

    return ( 0 == SHFileOperation( &fop ) ) ? errno_ok : errno_unaccess;
}

int32_t file_sys::is_file_exist( const char* file )
{
    if( nullptr == file || strlen( file ) == 0 )
    {
        set_err_no( errno_invalid_param );
        return false;
    }

    bool found = false;
    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile( file, &wfd );
    if( INVALID_HANDLE_VALUE != hFind )
    {
        if( !( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
        {
            found = true;
        }
        FindClose( hFind );
    }
    return found ? errno_ok : errno_general;;
}

int32_t file_sys::get_file_size( const char* file, uint64_t& bytes )
{
    if( nullptr == file || strlen( file ) == 0 )
    {
        return errno_invalid_param;
    }

    HANDLE hFind;
    WIN32_FIND_DATA fileInfo;
    hFind = FindFirstFile( file, &fileInfo );
    if( hFind != INVALID_HANDLE_VALUE )
    {
        auto size = static_cast< uint64_t >( fileInfo.nFileSizeHigh ) << ( sizeof( fileInfo.nFileSizeLow ) * 8 );
        size |= fileInfo.nFileSizeLow;
        FindClose( hFind );

        bytes = size;
        return errno_ok;
    }
    return errno_unaccess;
}

int32_t file_sys::copy_file( const char* src, const char* dst )
{
    if( nullptr == src || strlen( src ) == 0
     || nullptr == dst || strlen( dst ) == 0 )
    {
        return errno_invalid_param;
    }
    return ( TRUE == CopyFile( src, dst, FALSE ) ) ? errno_ok : errno_unaccess;
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
