/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2019 Barcelona Supercomputing Center (BSC)
*/

#ifndef TASK_CONTEXT_HPP
#define TASK_CONTEXT_HPP

#include <array>
#include <cassert>

#include "TaskingModel.hpp"
#include "util/ErrorHandler.hpp"

class TaskContext {
private:
	bool _blocking;
	void *_taskData;
	
public:
	inline TaskContext()
	{}
	
	inline TaskContext(bool blocking) :
		_blocking(blocking),
		_taskData(nullptr)
	{
		_taskData = getCurrentTaskData(blocking);
		assert(_taskData != nullptr);
	}
	
	inline TaskContext(bool blocking, void *taskData) :
		_blocking(blocking),
		_taskData(taskData)
	{
		assert(taskData != nullptr);
	}
	
	inline void *getTaskData() const
	{
		return _taskData;
	}
	
	inline void bindEvents(int num)
	{
		assert(num > 0);
		if (!_blocking) {
			TaskingModel::increaseCurrentTaskEventCounter(_taskData, num);
		}
	}
	
	inline void completeEvents(int num, bool allCompleted)
	{
		assert(num > 0);
		if (!_blocking) {
			TaskingModel::decreaseTaskEventCounter(_taskData, num);
		} else if (allCompleted) {
			TaskingModel::unblockTask(_taskData);
		}
	}
	
	inline void waitEventsCompletion()
	{
		assert(_blocking);
		TaskingModel::blockCurrentTask(_taskData);
	}
	
	inline static void *getCurrentTaskData(bool blocking)
	{
		if (blocking) {
			return TaskingModel::getCurrentBlockingContext();
		} else {
			return TaskingModel::getCurrentEventCounter();
		}
	}
};

#endif // TASK_CONTEXT_HPP

