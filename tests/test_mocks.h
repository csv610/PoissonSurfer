#pragma once
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

inline void DumpOutput( const char* format , ... ) {
    va_list args;
    va_start( args , format );
    vprintf( format , args );
    va_end( args );
}
inline void DumpOutput2( char* str , const char* format , ... ) {
    va_list args;
    va_start( args , format );
    vprintf( format , args );
    va_end( args );
    va_list args2;
    va_start( args2 , format );
    vsprintf( str , format , args2 );
    va_end( args2 );
    if( str[strlen(str)-1]=='\n' ) str[strlen(str)-1] = 0;
}
