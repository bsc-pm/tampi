/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef TASK_CONTEXT_HPP
#define TASK_CONTEXT_HPP

#include "util/error.hpp"

#include "RuntimeAPI.hpp"

#include <array>
#include <cassert>

class TaskContext {
private:
	bool _blocking;
	void *_taskData;
	
public:
	inline TaskContext(bool blocking) :
		_blocking(blocking),
		_taskData(nullptr)
	{
		if (_blocking) {
			_taskData = getCurrentBlockingContext();
		} else {
			_taskData = getCurrentEventCounter();
		}
		assert(_taskData != nullptr);
	}
	
	inline void bindedEvents(int num)
	{
		assert(num > 0);
		if (!_blocking) {
			increaseCurrentEventCounter(_taskData, num);
		}
	}
	
	inline void completedEvents(int num, bool allCompleted)
	{
		assert(num > 0);
		if (!_blocking) {
			decreaseEventCounter(_taskData, num);
		} else if (allCompleted) {
			unblockTask(_taskData);
		}
	}
	
	inline void waitEventsCompletion()
	{
		assert(_blocking);
		blockCurrentTask(_taskData);
	}
	
	inline static void prepareNonBlockingModeAPI()
	{
#if NOTIFY_EXTERNAL_EVENTS_API
		nanos6_notify_task_event_counter_api();
#endif
	}
	
private:

#if HAVE_BLOCKING_MODE
	inline static void *getCurrentBlockingContext()
	{
		return nanos6_get_current_blocking_context();
	}
	
	inline static void blockCurrentTask(void *blockingContext)
	{
		nanos6_block_current_task(blockingContext);
	}
	
	inline static void unblockTask(void *blockingContext)
	{
		nanos6_unblock_task(blockingContext);
	}
#else
	inline static void *getCurrentBlockingContext()
	{
		error::fail("Blocking mode is not supported");
		return 0;
	}
	
	inline static void blockCurrentTask(void *)
	{
		error::fail("Blocking mode is not supported");
	}
	
	inline static void unblockTask(void *)
	{
		error::fail("Blocking mode is not supported");
	}
#endif
	
#if HAVE_NONBLOCKING_MODE
	inline static void *getCurrentEventCounter()
	{
		return nanos6_get_current_event_counter();
	}
	
	inline static void increaseCurrentEventCounter(void *counter, int increment)
	{
		nanos6_increase_current_task_event_counter(counter, increment);
	}
	
	inline static void decreaseEventCounter(void *counter, int decrement)
	{
		nanos6_decrease_task_event_counter(counter, decrement);
	}
#else
	inline static void *getCurrentEventCounter()
	{
		error::fail("Non-blocking mode is not supported");
		return 0;
	}
	
	inline static void increaseCurrentEventCounter(void *, int)
	{
		error::fail("Non-blocking mode is not supported");
	}
	
	inline static void decreaseEventCounter(void *, int)
	{
		error::fail("Non-blocking mode is not supported");
	}
#endif
};

#endif // TASK_CONTEXT_HPP

