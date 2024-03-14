/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef BOOST_LOCK_FREE_QUEUE_HPP
#define BOOST_LOCK_FREE_QUEUE_HPP

#include <boost/version.hpp>

// Support for BOOST 1.67
#if BOOST_VERSION == 106700
#include <boost/next_prior.hpp>
#endif

#include <boost/lockfree/spsc_queue.hpp>

#include "EnvironmentVariable.hpp"
#include "ErrorHandler.hpp"
#include "SpinLock.hpp"
#include "SpinWait.hpp"


namespace tampi {

//! Class that provides the functionality of a lock free queue
template <typename T, size_t Size = 32*1024>
class BoostLockFreeQueue {
private:
	typedef boost::lockfree::spsc_queue<T, boost::lockfree::capacity<Size> > queue_t;

	//! Indicate whether the queue will have multiple producers. The producer
	//! side needs mutual exclusion if there are multiple producers
	const bool _multipleProducers;

	//! Envar indicating whether the execution should abort if the queue is full
	const EnvironmentVariable<bool> _fullFailure;

	//! Spinlock to add elements in the queue
	alignas(CacheAlignment) SpinLock _adderMutex;

	//! Single producer single consumer queue
	alignas(CacheAlignment) queue_t _queue;

public:
	BoostLockFreeQueue(bool multipleProducers = true) :
		_multipleProducers(multipleProducers),
		_fullFailure("TAMPI_QUEUES_FULL_FAILURE", false),
		_adderMutex(),
		_queue()
	{
	}

	//! \brief Push an element to the queue
	//!
	//! \param element The element to add
	void push(const T &element)
	{
		// Acquire the producer mutex if needed
		if (_multipleProducers)
			_adderMutex.lock();

		if (_fullFailure)
			pushOrFail(&element, 1);
		else
			pushCont(&element, 1);

		// Release the producer mutex if needed
		if (_multipleProducers)
			_adderMutex.unlock();
	}

	//! \brief Push elements to the queue
	//!
	//! \param elements The elements to add
	//! \param count The number of elements to add
	void push(const T elements[], size_t count)
	{
		// Acquire the producer mutex if needed
		if (_multipleProducers)
			_adderMutex.lock();

		if (_fullFailure)
			pushOrFail(elements, count);
		else
			pushCont(elements, count);

		// Release the producer mutex if needed
		if (_multipleProducers)
			_adderMutex.unlock();
	}

	//! \brief Pop multiple elements from the queue
	//!
	//! \param elements The array to store the retrieved elements
	//! \param count The maximum number of elements to retrieve
	//!
	//! \returns The number of elements retrieved
	size_t pop(T elements[], size_t count)
	{
		assert(elements != nullptr);
		return _queue.pop(elements, count);
	}

private:
	//! \brief Push elements to the queue or fail if full
	//!
	//! \param elements The elements to add
	//! \param count The number of elements to add
	void pushOrFail(const T elements[], size_t count)
	{
		if (_queue.push(elements, count) != count)
			ErrorHandler::fail("BoostLockFreeQueue is full");
	}

	//! \brief Push elements to the queue unconditionally
	//!
	//! \param elements The elements to add
	//! \param count The number of elements to add
	void pushCont(const T elements[], size_t count)
	{
		size_t pushed = _queue.push(elements, count);
		if (pushed == count)
			return;

		do {
			SpinWait::wait();
			pushed += _queue.push(&elements[pushed], count - pushed);
		} while (pushed < count);

		SpinWait::release();
	}
};

} // namespace tampi

#endif // BOOST_LOCK_FREE_QUEUE_HPP
