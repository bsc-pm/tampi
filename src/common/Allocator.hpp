/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <cassert>
#include <cstdlib>
#include <cstddef>

#include "TaskingModel.hpp"
#include "util/FixedSizeStack.hpp"
#include "util/SpinLock.hpp"
#include "util/Utils.hpp"

#include <boost/lockfree/spsc_queue.hpp>


namespace tampi {

//! Class that implements an object allocator. The allocator is suitable for
//! scenarios where multiple CPUs will demand memory objects. The allocator
//! maintains a SPSC queue with all the pre-allocated objects. Each CPU has
//! a small cache where holds some objects. Initially, the caches are empty,
//! so the consumers have to retrieve some objects from the SPSC queue and
//! temporarily store them into the cache. There should only be one entity
//! freeing objects, which returns the objects back to the SPSC queue
template <typename T>
class ObjAllocator {
	typedef Padded<T> Data;
	typedef boost::lockfree::spsc_queue<T *, boost::lockfree::fixed_sized<true>> Queue;

	static constexpr size_t MaxCaches = MaxSystemCPUs;
	static constexpr size_t BatchSize = 64;

	//! The number of objects
	alignas(CacheAlignment) const size_t _capacity;

	//! The number of caches (one per CPU)
	const size_t _ncaches;

	//! The pointer to the array of padded objects
	Data *_data;

	//! The caches: one per CPU
	alignas(CacheAlignment) PaddedArray<FixedSizeStack<T *, BatchSize>, MaxCaches> _caches;

	//! The central SPSC queue
	alignas(CacheAlignment) Queue _central;

	//! The spinlock used to access the consumer side of the SPSC queue
	alignas(CacheAlignment) SpinLock _mutex;

protected:
	friend class Allocator;

	//! The instance for this template allocator
	static ObjAllocator *_instance;

public:
	//! \brief Constructs an allocator
	//!
	//! \param capacity The number of objects to allocate
	ObjAllocator(size_t capacity) :
		_capacity(capacity),
		_ncaches(TaskingModel::getNumLogicalCPUs()),
		_data(Memory::alignedAlloc<Data>(capacity)),
		_caches(),
		_central(capacity)
	{
		if (_ncaches > MaxCaches)
			ErrorHandler::fail(__func__, ": Maximum number of queues exceeded. Runtime got: ", _ncaches, ", TAMPI was compiled with maximum: ", MaxCaches);

		for (size_t o = 0; o < _capacity; ++o)
			_central.push(&(_data[o].get()));
	}

	//! \brief Destroys an allocator
	~ObjAllocator()
	{
		Memory::alignedFree<Data>(_data, _capacity);
	}

	//! \brief Allocates and constructs an object
	//!
	//! \param args The arguments for constructing the object
	//!
	//! \returns The pointer to the object
	template <typename... Args>
	T *alloc(Args &&... args)
	{
		T *object = nullptr;

		size_t cache = TaskingModel::getCurrentLogicalCPU();
		assert(cache < _ncaches);

		if (!_caches[cache].pop(object)) {
			T *objects[BatchSize];

			_mutex.lock();
			size_t n = _central.pop(objects, BatchSize);
			_mutex.unlock();

			if (n == 0)
				ErrorHandler::fail("Unavailable allocator objects");

			if (n > 1)
				_caches[cache].push(&objects[1], n-1);

			object = objects[0];
		}
		assert(object != nullptr);

		new (object) T(std::forward<Args>(args)...);
		return object;
	}

	//! \brief Frees several objects
	//!
	//! This operation must be executed by a single entity, i.e., the polling
	//! task. Calling this function from other entities is invalid. See the
	//! 'localFree' operation instead
	//!
	//! \param objects The objects to free
	//! \param n The number of objects to free
	void free(T *objects[], size_t n)
	{
		for (size_t o = 0; o < n; ++o)
			objects[o]->~T();

		[[maybe_unused]] size_t pushed;
		pushed = _central.push(objects, n);
		assert(pushed == n);
	}

	//! \brief Frees several objects into the local cache
	//!
	//! \param objects The objects to free
	//! \param n The number of objects to free
	void localFree(T *objects[], size_t n)
	{
		for (size_t o = 0; o < n; ++o)
			objects[o]->~T();

		size_t cache = TaskingModel::getCurrentLogicalCPU();
		assert(cache < _ncaches);

		_caches[cache].push(objects, n);
	}
};

template <typename T>
ObjAllocator<T> *ObjAllocator<T>::_instance = nullptr;

//! Class that encapsulates all allocators
class Allocator {
	static constexpr size_t OperationCapacity = 64*1000;
	static constexpr size_t CollOperationCapacity = 8*1000;

public:
	//! \brief Initialize all the allocators
	static void initialize();

	//! \brief Finalize all the allocators
	static void finalize();

	//! \brief Allocates and constructs an object
	//!
	//! \param args The arguments for constructing the object
	//!
	//! \returns The pointer to the object
	template <typename T, typename... Args>
	static T *alloc(Args &&... args)
	{
		assert(ObjAllocator<T>::_instance != nullptr);
		return ObjAllocator<T>::_instance->alloc(std::forward<Args>(args)...);
	}

	//! \brief Frees several objects
	//!
	//! This operation must be executed by a single entity, i.e., the polling
	//! task. Calling this function from other entities is invalid
	//!
	//! \param objects The objects to free
	//! \param n The number of objects to free
	template <typename T>
	static void free(T *objects[], size_t n)
	{
		assert(ObjAllocator<T>::_instance != nullptr);
		ObjAllocator<T>::_instance->free(objects, n);
	}
};

} // namespace tampi

#endif // ALLOCATOR_HPP
