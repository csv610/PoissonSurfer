#ifndef HASH_INCLUDED
#define HASH_INCLUDED
#include <unordered_map>

template< class Key , class T , class Hash = std::hash< Key > , class Pred = std::equal_to< Key > , class Alloc = std::allocator< std::pair< const Key , T > > >
using hash_map = std::unordered_map< Key , T , Hash , Pred , Alloc >;

#endif // HASH_INCLUDED
