
#if defined WIN32 || defined WIN64
extern _declspec(dllexport) int test_dynlib_add( int a, int b )
{
    return a + b;
}
#else // WIN32
extern int test_dynlib_add( int a, int b )
{
    return a + b;
}
#endif // WIN32
