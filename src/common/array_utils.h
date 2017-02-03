
#ifndef ARRAY_UTILS_H
#define ARRAY_UTILS_H

#include <cassert>
#include <mpi.h>

#include <array>
#include <vector>

namespace nanos {
namespace utils {

template< typename T >
struct transform_to;

template< typename T >
struct transform_to< std::vector<T> >
{
	std::vector<T> operator()( size_t count, const T* values )
	{
		auto begin = values;
		auto end = begin + count;
		return std::vector<T>( begin, end );
	}
};

template < typename T, size_t size >
struct transform_to< std::array<T,size> >
{
	const std::array<T,size>& operator()( size_t count, const T* values )
	{
		assert( size <= count );
		return *reinterpret_cast<const std::array<T,size>*>(values);
	}
};

template < typename T, typename Size >
std::reverse_iterator<T*> rbegin( T* array, Size length ) {
   return std::reverse_iterator<T*>(array+length-1);
}

template < typename T, typename Size >
std::reverse_iterator<const T*> crbegin( const T* array, Size length ) {
   return std::reverse_iterator<T*>(array+length-1);
}

template < typename T, typename Size >
std::reverse_iterator<T*> rend( T* array, Size length ) {
   return std::reverse_iterator<T*>(array-1);
}

template < typename T, typename Size >
std::reverse_iterator<const T*> crend( const T* array, Size length ) {
   return std::reverse_iterator<T*>(array-1);
}

} // namespace utils
} // namespace nanos

#endif // ARRAY_UTILS_H

