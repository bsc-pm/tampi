/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef UTILS_HPP
#define UTILS_HPP

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cassert>
#include <cstdint>
#include <type_traits>

#ifndef CACHELINE_SIZE
#define CACHELINE_SIZE 64
#endif

#ifndef MAX_SYSTEM_CPUS
#define MAX_SYSTEM_CPUS 50
#endif


namespace tampi {

//! Cache alignment is the size of two cache lines to avoid
//! false sharing between objects
constexpr size_t CacheAlignment = CACHELINE_SIZE * 2;

//! The maximum number of CPUs in the system
constexpr size_t MaxSystemCPUs = MAX_SYSTEM_CPUS;

//! Class that provides padding for a type
template <class T, size_t Size = CacheAlignment>
class Padded {
	//! \brief Computes the neareast integer multiple of a given value
	//!
	//! \param x The value to round
	//! \param y The value which the result should be multiple of
	//!
	//! \returns The nearest multiple integer
	static constexpr size_t roundup(size_t const x, size_t const y)
	{
		return (((x + (y - 1)) / y) * y);
	}

	//! The actual object
	T _value;

	//! The extra space
	uint8_t _padding[roundup(sizeof(T), Size) - sizeof(T)];

public:
	Padded() {}

	Padded(T value) : _value(value) {}

	template<typename... Args>
	Padded(Args&&... args) : _value(std::forward<Args>(args)...) {}

	//! \brief Get a reference to the object
	T &get()
	{
		return _value;
	}

	//! \brief Get a const reference to the object
	const T &get() const
	{
		return _value;
	}
};

//! Class that provides a padded array
template <class T, size_t Capacity, size_t Padding = CacheAlignment>
class PaddedArray {
	//! The array of objects
	Padded<T, Padding> _data[Capacity];

public:
	PaddedArray()
	{
		assert((uintptr_t) _data % CacheAlignment == 0);
	}

	T &operator[](size_t i)
	{
		assert(i < Capacity);
		return _data[i].get();
	}

	const T &operator[](size_t i) const
	{
		assert(i < Capacity);
		return _data[i].get();
	}
};

//! Class providing uninitialized array memory for a type
template <class T, size_t N = 1>
class Uninitialized {
	typename std::aligned_storage<sizeof(T), alignof(T)>::type _data[N];

public:
	T *get()
	{
		return reinterpret_cast<T *>(&_data);
	}

	operator T *()
	{
		return get();
	}

	T &operator[](size_t idx)
	{
		return *(get() + idx);
	}

	const T &operator[](size_t idx) const
	{
		return *(get() + idx);
	}
};

} // namespace tampi

#endif // UTILS_HPP
