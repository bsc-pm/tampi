/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef SPIN_LOCK_HPP
#define SPIN_LOCK_HPP

#include <atomic>

#include "SpinWait.hpp"
#include "Utils.hpp"


namespace tampi {

//! Class that implements a ticket array spinlock that focuses on
//! avoiding the overhead when there are many threads trying to
//! acquire the lock at the same time
class SpinLock {
private:
	static constexpr size_t Size = MaxSystemCPUs;

	alignas(CacheAlignment) PaddedArray<std::atomic<size_t>, Size> _buffer;
	alignas(CacheAlignment) std::atomic<size_t> _head;
	alignas(CacheAlignment) size_t _next;

public:
	SpinLock() :
		_head(0),
		_next(0)
	{
		for (size_t i = 0; i < Size; ++i)
			std::atomic_init(&_buffer[i], (size_t) 0);
	}

	//! \brief Aquire the spinlock
	void lock()
	{
		const size_t head = _head.fetch_add(1, std::memory_order_relaxed);
		const size_t idx = head % Size;
		while (_buffer[idx].load(std::memory_order_relaxed) != head) {
			SpinWait::wait();
		}
		SpinWait::release();

		std::atomic_thread_fence(std::memory_order_acquire);
	}

	//! \brief Try to acquire the spinlock
	//!
	//! \returns Whether the spinlock was acquired
	bool try_lock()
	{
		size_t head = _head.load(std::memory_order_relaxed);
		const size_t idx = head % Size;
		if (_buffer[idx].load(std::memory_order_relaxed) != head)
			return false;

		return std::atomic_compare_exchange_strong_explicit(
			&_head, &head, head + 1,
			std::memory_order_acquire,
			std::memory_order_relaxed);
	}

	//! \brief Release the spinlock
	void unlock()
	{
		const size_t idx = ++_next % Size;
		_buffer[idx].store(_next, std::memory_order_release);
	}
};

} // namespace tampi

#endif // SPIN_LOCK_HPP
