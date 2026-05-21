#ifndef ARRAY_INCLUDED
#define ARRAY_INCLUDED

#include <vector>
#include <stdexcept>
#include <string>

#define ARRAY_DEBUG 0
#ifdef _WIN64
#define ASSERT( x ) { if( !( x ) ) __debugbreak(); }
#else // !_WIN64
#ifdef _WIN32
#define ASSERT( x ) { if( !( x ) ) _asm{ int 0x03 } }
#else // !_WIN32
#define ASSERT( x ) { if( !( x ) ) throw std::runtime_error("Assertion failed: " #x); }
#endif // _WIN32
#endif // _WIN64

// Code from http://stackoverflow.com
inline void* aligned_malloc( size_t size , size_t align )
{
	// Align enough for the data, the alignment padding, and room to store a pointer to the actual start of the memory
	void*  mem = malloc( size + align + sizeof( void* ) );
	// The position at which we could potentially start addressing
	char* amem = static_cast<char*>(mem) + sizeof( void* );
	// Add align-1 to the start of the address and then zero out at most of the first align-1 bits.
	amem = reinterpret_cast<char*>( ( reinterpret_cast<size_t>( static_cast<char*>(amem) + (align-1) ) ) & ~( align-1 ) );
	// Pre-write the actual address
	reinterpret_cast<void**>(amem)[-1] = mem;
	return amem;
}
inline void aligned_free( void* mem ) { if(mem) free( reinterpret_cast<void**>(mem)[-1] ); }

#if ARRAY_DEBUG
#pragma message ( "[WARNING] Array debugging is enabled" )
#include "Array.inl"
#define      Pointer( ... )      Array< __VA_ARGS__ >
#define ConstPointer( ... ) ConstArray< __VA_ARGS__ >
template< class C > void        FreePointer( Array< C >& a ){ a.Free( ); }
template< class C > void AlignedFreePointer( Array< C >& a ){ a.Free( ); }
template< class C > void       VFreePointer( Array< C >& a ){ a.Free( ); }
template< class C > void      DeletePointer( Array< C >& a ){ a.Delete( ); }

template< class C > Array< C >          NewPointer(                  size_t size ,                 const char* name=NULL ){ return Array< C >::New         (     size ,                     name ); }
template< class C > Array< C >        AllocPointer(                  size_t size ,                 const char* name=NULL ){ return Array< C >::Alloc       (     size ,             false , name ); }
template< class C > Array< C > AlignedAllocPointer(                  size_t size , size_t alignment , const char* name=NULL ){ return Array< C >::AlignedAlloc(     size , alignment , false , name ); }
template< class C > Array< C >      ReAllocPointer( Array< C >&  a , size_t size ,                 const char* name=NULL ){ return Array< C >::ReAlloc     ( a , size ,             false , name ); }

template< class C > Array< C > NullPointer(){ return Array< C >( ); }

template< class C >       C* PointerAddress(      Array< C >& a ) { return a.pointer(); }
template< class C > const C* PointerAddress( ConstArray< C >& a ) { return a.pointer(); }
template< class C >      Array< C > GetPointer(       C& c ) { return      Array< C >::FromPointer( &c , 1 ); }
template< class C > ConstArray< C > GetPointer( const C& c ) { return ConstArray< C >::FromPointer( &c , 1 ); }
template< class C >      Array< C > GetPointer(       std::vector< C >& v ){ return      Array< C >::FromPointer( &v[0] , v.size() ); }
template< class C > ConstArray< C > GetPointer( const std::vector< C >& v ){ return ConstArray< C >::FromPointer( &v[0] , v.size() ); }

#else // !ARRAY_DEBUG
#define      Pointer( ... )       __VA_ARGS__*
#define ConstPointer( ... ) const __VA_ARGS__*

#define        FreePointer( ... ) { if( __VA_ARGS__ )         free( __VA_ARGS__ ) ,                   __VA_ARGS__ = NULL; }
#define AlignedFreePointer( ... ) { if( __VA_ARGS__ ) aligned_free( __VA_ARGS__ ) ,                   __VA_ARGS__ = NULL; }
#define      DeletePointer( ... ) { if( __VA_ARGS__ )      delete[] __VA_ARGS__ ,                     __VA_ARGS__ = NULL; }

template< class C > C*          NewPointer(        size_t size ,                    const char* name=NULL ){ return new C[size]; }
template< class C > C*        AllocPointer(        size_t size ,                    const char* name=NULL ){ return (C*)        malloc(        sizeof(C) * size             ); }
template< class C > C* AlignedAllocPointer(        size_t size , size_t alignment , const char* name=NULL ){ return (C*)aligned_malloc(        sizeof(C) * size , alignment ); }
template< class C > C*      ReAllocPointer( C* c , size_t size ,                    const char* name=NULL ){ return (C*)       realloc( c    , sizeof(C) * size             ); }

template< class C > C* NullPointer(){ return NULL; }

template< class C >       C* PointerAddress(       C* c ){ return c; }
template< class C > const C* PointerAddress( const C* c ){ return c; }
template< class C >       C* GetPointer(       C& c ){ return &c; }
template< class C > const C* GetPointer( const C& c ){ return &c; }
template< class C >       C* GetPointer(       std::vector< C >& v ){ return &v[0]; }
template< class C > const C* GetPointer( const std::vector< C >& v ){ return &v[0]; }
#endif // ARRAY_DEBUG
#endif // ARRAY_INCLUDED
