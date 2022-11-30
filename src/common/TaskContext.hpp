/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2022 Barcelona Supercomputing Center (BSC)
*/

#ifndef TASK_CONTEXT_HPP
#define TASK_CONTEXT_HPP

#include <array>
#include <cassert>

#include "TaskingModel.hpp"
#include "util/ErrorHandler.hpp"


namespace tampi {

//! Class that represents the context of a task and provides
//! several functions to bind events to the task, either in
//! a blocking or non-blocking manner
class TaskContext {
private:
	//! Indicate whether the task is using the blocking mode
	bool _blocking;

	//! An opaque pointer to the blocking context or events counter 
	//! depending on the value of the previous field
	void *_taskData;

public:
	//! \brief Construct a task context
	inline TaskContext()
	{
	}

	//! \brief Construct a task context
	//!
	//! \param blocking Whether the task is using the blocking mode
	inline TaskContext(bool blocking) :
		_blocking(blocking),
		_taskData(nullptr)
	{
		_taskData = getCurrentTaskData(blocking);
		assert(_taskData != nullptr);
	}

	//! \brief Construct a task context
	//!
	//! \param blocking Whether the task is using the blocking mode
	//! \param taskData The task blocking context or events counter
	inline TaskContext(bool blocking, void *taskData) :
		_blocking(blocking),
		_taskData(taskData)
	{
		assert(taskData != nullptr);
	}

	//! \brief Get the task blocking context or events counter
	//!
	//! \returns An opaque pointer to the context or counter
	inline void *getTaskData() const
	{
		return _taskData;
	}

	//! \brief Bind some events to the task
	//!
	//! \param num The number of events to bind
	inline void bindEvents(int num)
	{
		assert(num > 0);
		if (!_blocking) {
			TaskingModel::increaseCurrentTaskEventCounter(_taskData, num);
		}
	}

	//! \brief Complete some events of the task
	//!
	//! \param num The number of events to complete
	//! \param allCompleted Whether these are the last events to complete
	inline void completeEvents(int num, bool allCompleted)
	{
		assert(num > 0);
		if (!_blocking) {
			TaskingModel::decreaseTaskEventCounter(_taskData, num);
		} else if (allCompleted) {
			// Unblock the task when all its events completed
			TaskingModel::unblockTask(_taskData);
		}
	}

	//! \brief Wait until all its events finished
	//!
	//! This function blocks the current task until its events
	//! have been completed. Note this call is only allowed when
	//! the task is using the blocking mode
	inline void waitEventsCompletion()
	{
		assert(_blocking);
		TaskingModel::blockCurrentTask(_taskData);
	}

	//! \brief Indicate whether the task is using the blocking mode
	inline bool isBlocking() const
	{
		return _blocking;
	}

private:
	//! \brief Get the blocking context or events counter
	//!
	//! \param blocking Whether the task is using the blocking mode
	//!
	//! \returns An opaque pointer to the context or counter
	inline static void *getCurrentTaskData(bool blocking)
	{
		if (blocking) {
			return TaskingModel::getCurrentBlockingContext();
		} else {
			return TaskingModel::getCurrentEventCounter();
		}
	}
};

} // namespace tampi

#endif // TASK_CONTEXT_HPP

