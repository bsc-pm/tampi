/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef MULTI_LOCK_FREE_QUEUE_HPP
#define MULTI_LOCK_FREE_QUEUE_HPP

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cstdlib>
#include <cstddef>
#include <cstdint>

#include "EnvironmentVariable.hpp"
#include "Utils.hpp"


namespace tampi {

enum class MultiQueuePopPolicy {
	//! Cyclic pop operation that tries to remove the same number
	//! of elements from all the queues. The policy focuses on
	//! the fairness when retriving elements
	CyclicRoundRobin = 0,
	//! Block pop operation that tries to remove all elements from
	//! the same queue to achieve the most optimal copy possible.
	//! The policy does not focus on fairness but performance
	BlockRoundRobin,
};

template <typename T, MultiQueuePopPolicy Policy, size_t Capacity = 32*1024, size_t MaxQueues = MaxSystemCPUs>
class MultiLockFreeQueue {
	typedef uint64_t counter_t;
	typedef std::atomic<counter_t> atomic_counter_t;
	typedef T* const __restrict__ data_ptr_t;

	alignas(CacheAlignment) const size_t _capacity;
	const size_t _queues;
	data_ptr_t _data;

	//! Envar indicating whether the execution should abort if the queue is full
	const EnvironmentVariable<bool> _fullFailure;

	alignas(CacheAlignment) PaddedArray<atomic_counter_t, MaxQueues> _tail;

	// The more packed the better
	alignas(CacheAlignment) counter_t _remaining[MaxQueues];

	alignas(CacheAlignment) PaddedArray<atomic_counter_t, MaxQueues> _head;

	// The elements of this array should be padded to avoid false sharing
	alignas(CacheAlignment) PaddedArray<counter_t, MaxQueues> _available;

	alignas(CacheAlignment) size_t _totalRemaining;

public:
	MultiLockFreeQueue() :
		_capacity(Capacity),
		_queues(TaskingModel::getNumLogicalCPUs()),
		_data(Memory::alignedAllocate<T>(_queues * _capacity)),
		_fullFailure("TAMPI_QUEUES_FULL_FAILURE", true),
		_tail(),
		_head(),
		_available(),
		_totalRemaining(0)
	{
		if (_queues > MaxQueues)
			ErrorHandler::fail("Maximum number of queues exceeded");

		for (size_t i = 0; i < _queues; i++) {
			_tail[i].store(0);
			_head[i].store(0);
			_available[i] = 0;
			_remaining[i] = 0;
		}
	}

	~MultiLockFreeQueue()
	{
		Memory::alignedDeallocate<T>(_data, _queues * _capacity);
	}

private:
	size_t getRemaining(size_t queue) const
	{
		assert(queue < _queues);
		size_t head = _head[queue].load(std::memory_order_acquire);
		size_t tail = _tail[queue].load(std::memory_order_relaxed);
		assert(head < _capacity);
		assert(tail < _capacity);
		return (tail <= head) ? head - tail : (head - tail) + _capacity;
	}

	size_t updateRemaining()
	{
		size_t total = 0;
		for (size_t q = 0; q < _queues; q++) {
			size_t val = getRemaining(q);
			_remaining[q] = val;
			total += val;
		}
		return total;
	}

	size_t getAvailable(size_t queue) const
	{
		assert(queue < _queues);
		return _capacity - (getRemaining(queue) + 1);
	}

	void unsafePush(size_t queue, T const __restrict__ elems[], size_t n)
	{
		assert(queue < _queues);
		assert(n > 0);
		assert(n <= _available[queue]);

		_available[queue] -= (counter_t) n;
		data_ptr_t data = _data + (queue * _capacity);

		size_t head = _head[queue].load(std::memory_order_relaxed);

		size_t n0 = std::min(n, _capacity - head);
		for (size_t i = 0; i < n0; i++) {
			data[head + i] = elems[i];
		}
		for (size_t i = 0; i < (n - n0); i++) {
			data[i] = elems[n0 + i];
		}

		head = (head + n) % _capacity;
		_head[queue].store((counter_t) head, std::memory_order_release);
	}

	void unsafePop(size_t queue, T __restrict__ values[], size_t n)
	{
		assert(queue < _queues);
		assert(n > 0);
		assert(n <= _remaining[queue]);

		_remaining[queue] -= n;
		data_ptr_t data = _data + (queue * _capacity);

		size_t tail = _tail[queue].load(std::memory_order_relaxed);

		size_t n0 = std::min(n, _capacity - tail);
		for (size_t i = 0; i < n0; i++)
			values[i] = data[tail + i];

		for (size_t i = 0; i < (n - n0); i++)
			values[n0 + i] = data[i];

		tail = (tail + n) % _capacity;
		_tail[queue].store((counter_t) tail, std::memory_order_release);
	}

	size_t cyclicRoundRobinPop(T __restrict__ values[], size_t n)
	{
		static size_t queue = 0;
		if (n == 0)
			return 0;

		if (_totalRemaining < n) {
			_totalRemaining = updateRemaining();

			if (_totalRemaining == 0)
				return 0;

			n = std::min<size_t>(n, _totalRemaining);
		}

		size_t remaining[MaxQueues];

		for (size_t q = 0; q < _queues; ++q)
			remaining[q] = _remaining[q];

		const size_t queue0 = queue;

		size_t missing = n;
		while (missing > 0) {
			size_t perQueue = std::max<size_t>(missing / _queues, 1);
			for (size_t q = 0; missing > 0 && q < _queues; ++q) {
				assert(queue < _queues);
				size_t avail = std::min<size_t>(remaining[queue], perQueue);
				if (avail) {
					remaining[queue] -= avail;
					missing -= avail;
				}
				queue = (queue + 1) % _queues;
			}
		}

		_totalRemaining -= n;

		size_t q = queue0;
		size_t i = 0;
		while (i < n) {
			assert(q < _queues);
			size_t assigned = _remaining[q] - remaining[q];
			if (assigned > 0) {
				unsafePop(q, values + i, assigned);
				i += assigned;
			}
			q = (q + 1) % _queues;
		}

		return n;
	}

	size_t blockRoundRobinPop(T __restrict__ values[], size_t n)
	{
		static size_t queue = 0;
		if (n == 0)
			return 0;

		if (_totalRemaining < n) {
			_totalRemaining = updateRemaining();
			if (_totalRemaining == 0)
				return 0;

			n = std::min<size_t>(n, _totalRemaining);
		}

		_totalRemaining -= n;

		size_t i = 0;
		while (i < n) {
			assert(queue < _queues);
			size_t assigned = std::min<size_t>(_remaining[queue], n - i);
			if (assigned == 0) {
				queue = (queue + 1) % _queues;
				continue;
			}
			unsafePop(queue, values + i, assigned);
			i += assigned;
		}

		return n;
	}

	bool push(const T &element, size_t queue)
	{
		if (_available[queue] == 0) {
			_available[queue] = getAvailable(queue);
			if (_available[queue] == 0)
				return false;
		}
		unsafePush(queue, &element, 1);

		return true;
	}

public:
	bool empty()
	{
		if (_totalRemaining != 0)
			return false;

		_totalRemaining = updateRemaining();
		return (_totalRemaining == 0);
	}

	void push(const T &element)
	{
		size_t queue = TaskingModel::getCurrentLogicalCPU();
		assert(queue < _queues);

		if (_fullFailure) {
			if (!push(element, queue))
				ErrorHandler::fail("MultiLockFreeQueue is full");
		} else {
			if (push(element, queue))
				return;

			do {
				SpinWait::wait();
			} while (!push(element, queue));
			SpinWait::release();
		}
	}

	bool pop(T &element)
	{
		return pop(&element, 1) == 1;
	}

	size_t pop(T __restrict__ values[], size_t n)
	{
		if constexpr (Policy == MultiQueuePopPolicy::CyclicRoundRobin)
			return cyclicRoundRobinPop(values, n);
		else if constexpr (Policy == MultiQueuePopPolicy::BlockRoundRobin)
			return blockRoundRobinPop(values, n);
		return 0;
	}
};

} // namespace tampi

#endif // MULTI_LOCK_FREE_QUEUE_HPP
