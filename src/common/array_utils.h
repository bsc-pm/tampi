
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

} // namespace utils
} // namespace nanos

#endif // ARRAY_UTILS_H

