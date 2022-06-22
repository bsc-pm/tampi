/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2020 Barcelona Supercomputing Center (BSC)
*/

#ifndef LOCK_FREE_QUEUE_HPP
#define LOCK_FREE_QUEUE_HPP

#include <functional>
#include <mutex>

#include <boost/version.hpp>

// Support for BOOST 1.67
#if BOOST_VERSION == 106700
#include <boost/next_prior.hpp>
#endif

#include <boost/lockfree/spsc_queue.hpp>

#include "SpinLock.hpp"


namespace tampi {
namespace util {

//! Class that provides the functionality of a lock free queue
template <typename T, size_t Size = 32*1024>
class LockFreeQueue {
private:
	typedef boost::lockfree::spsc_queue<T, boost::lockfree::capacity<Size> > queue_t;
	typedef std::function<void()> ProgressFunction;

	//! Spinlock to add elements in the queue
	SpinLock _adderMutex;

	//! Single producer single consumer queue
	queue_t _queue;

public:
	inline LockFreeQueue() :
		_adderMutex(),
		_queue()
	{
	}

	//! \brief Add an element to the queue
	//!
	//! \param element The element to add
	//! \param progressFunction The function that should be called to
	//!                         consume elements of the queue when full
	inline void add(const T &element, ProgressFunction progressFunction)
	{
		std::lock_guard<SpinLock> guard(_adderMutex);

		int pushed = 0;
		do {
			pushed = _queue.push(element);
			if (pushed < 1) {
				progressFunction();
			}
		} while (pushed == 0);
	}

	//! \brief Add multiple elements to the queue
	//!
	//! \param elements An array of the elements to add
	//! \param count The number of elements to add
	//! \param progressFunction The function that should be called to
	//!                         consume elements of the queue when full
	inline void add(const T elements[], int count, ProgressFunction progressFunction)
	{
		assert(elements != nullptr);
		std::lock_guard<SpinLock> guard(_adderMutex);

		int pushed = 0;
		do {
			pushed += _queue.push(&elements[pushed], count - pushed);
			if (pushed < count) {
				progressFunction();
			}
		} while (pushed < count);
	}

	//! \brief Retrieve multiple elements from the queue
	//!
	//! \param elements The array to store the retrieved elements
	//! \param count The maximum number of elements to retrieve
	//!
	//! \returns The number of elements retrieved
	inline int retrieve(T elements[], int count)
	{
		assert(elements != nullptr);
		return _queue.pop(elements, count);
	}
};

} // namespace util
} // namespace tampi

#endif // LOCK_FREE_QUEUE_HPP
