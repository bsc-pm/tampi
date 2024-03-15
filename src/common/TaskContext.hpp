/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef TASK_CONTEXT_HPP
#define TASK_CONTEXT_HPP

#include <cassert>

#include "TaskingModel.hpp"

namespace tampi {

//! Class that represents the context of a task and provides
//! several functions to bind events to the task, either in
//! a blocking or non-blocking manner
class TaskContext {
private:
	//! Indicate whether the task is using the blocking mode
	bool _blocking;

	//! The task handle
	TaskingModel::task_handle_t _taskHandle;

public:
	//! \brief Construct a task context
	TaskContext() :
		_blocking(false),
		_taskHandle(nullptr)
	{
	}

	//! \brief Construct a task context
	//!
	//! \param blocking Whether the task is using the blocking mode
	TaskContext(bool blocking) :
		_blocking(blocking),
		_taskHandle(nullptr)
	{
		_taskHandle = TaskingModel::getCurrentTask();
		assert(_taskHandle != nullptr);
	}

	//! \brief Construct a task context
	//!
	//! \param blocking Whether the task is using the blocking mode
	//! \param taskHandle The task handle
	TaskContext(bool blocking, TaskingModel::task_handle_t taskHandle) :
		_blocking(blocking),
		_taskHandle(taskHandle)
	{
		assert(taskHandle != nullptr);
	}

	//! \brief Get the task handle
	TaskingModel::task_handle_t getTaskHandle() const
	{
		return _taskHandle;
	}

	//! \brief Bind some events to the task
	//!
	//! \param num The number of events to bind
	void bindEvents(int num)
	{
		assert(num > 0);
		if (!_blocking) {
			TaskingModel::increaseCurrentTaskEvents(_taskHandle, num);
		}
	}

	//! \brief Complete some events of the task
	//!
	//! \param num The number of events to complete
	//! \param allCompleted Whether these are the last events to complete
	void completeEvents(int num, bool allCompleted)
	{
		assert(num > 0);
		if (!_blocking) {
			TaskingModel::decreaseTaskEvents(_taskHandle, num);
		} else if (allCompleted) {
			// Unblock the task when all its events completed
			TaskingModel::unblockTask(_taskHandle);
		}
	}

	//! \brief Wait until all its events finished
	//!
	//! This function blocks the current task until its events
	//! have been completed. Note this call is only allowed when
	//! the task is using the blocking mode
	void waitEventsCompletion()
	{
		assert(_blocking);
		TaskingModel::blockCurrentTask(_taskHandle);
	}

	//! \brief Indicate whether the task is using the blocking mode
	bool isBlocking() const
	{
		return _blocking;
	}
};

} // namespace tampi

#endif // TASK_CONTEXT_HPP

