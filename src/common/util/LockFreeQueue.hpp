/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef LOCK_FREE_QUEUE_HPP
#define LOCK_FREE_QUEUE_HPP

#include <mutex>

#include <boost/version.hpp>

// Support for BOOST 1.67
#if BOOST_VERSION == 106700
#include <boost/next_prior.hpp>
#endif

#include <boost/lockfree/spsc_queue.hpp>

#include "SpinLock.hpp"


namespace tampi {

//! Class that provides the functionality of a lock free queue
template <typename T, size_t Size = 32*1024>
class LockFreeQueue {
private:
	typedef boost::lockfree::spsc_queue<T, boost::lockfree::capacity<Size> > queue_t;

	//! Spinlock to add elements in the queue
	SpinLock _adderMutex;

	//! Single producer single consumer queue
	queue_t _queue;

public:
	LockFreeQueue() :
		_adderMutex(),
		_queue()
	{
	}

	//! \brief Push an element to the queue
	//!
	//! \param element The element to add
	//! \param progressFunction The function that should be called to
	//!                         consume elements of the queue when full
	template <typename ProgressFunction>
	void push(const T &element, ProgressFunction progressFunction)
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

	//! \brief Push multiple elements to the queue
	//!
	//! \param elements An array of the elements to add
	//! \param count The number of elements to add
	//! \param progressFunction The function that should be called to
	//!                         consume elements of the queue when full
	template <typename ProgressFunction>
	void push(const T elements[], int count, ProgressFunction progressFunction)
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

	//! \brief Pop multiple elements from the queue
	//!
	//! \param elements The array to store the retrieved elements
	//! \param count The maximum number of elements to retrieve
	//!
	//! \returns The number of elements retrieved
	int pop(T elements[], int count)
	{
		assert(elements != nullptr);
		return _queue.pop(elements, count);
	}
};

} // namespace tampi

#endif // LOCK_FREE_QUEUE_HPP
