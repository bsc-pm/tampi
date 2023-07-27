/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef UTILS_HPP
#define UTILS_HPP

#include <config.h>

#include <cstdint>
#include <type_traits>

#ifndef CACHELINE_SIZE
#define CACHELINE_SIZE 64
#endif

#ifndef MAX_SYSTEM_CPUS
#define MAX_SYSTEM_CPUS 50
#endif


namespace tampi {
namespace util {

//! Class that provides padding for a type
template <class T, size_t Size = CACHELINE_SIZE>
class Padded : public T {
	using T::T;

	//! \brief Computes the neareast integer multiple of a given value
	//!
	//! \param x The value to round
	//! \param y The value which the result should be multiple of
	//!
	//! \returns The nearest multiple integer
	constexpr static size_t roundup(size_t const x, size_t const y)
	{
		return (((x + (y - 1)) / y) * y);
	}

	//! The actual padding
	uint8_t padding[roundup(sizeof(T), Size) - sizeof(T)];

public:
	//! \brief Get a pointer to the basetype object
	//!
	//! \returns A pointer to the basetype object
	inline T *ptr_to_basetype()
	{
		return (T *) this;
	}
};

//! Class providing uninitialized memory for a type
template <class T>
using Uninitialized = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

} // namespace util
} // namespace tampi

#endif // UTILS_HPP
