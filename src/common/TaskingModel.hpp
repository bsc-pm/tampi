/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2019-2020 Barcelona Supercomputing Center (BSC)
*/

#ifndef TASKING_MODEL_HPP
#define TASKING_MODEL_HPP

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>

#include "Symbol.hpp"
#include "TaskingModelAPI.hpp"
#include "util/EnvironmentVariable.hpp"


class TaskingModel {
public:
	typedef void (*polling_function_t)(void *args);
	typedef size_t polling_handle_t;

private:
	static register_polling_service_t *_registerPollingService;
	static unregister_polling_service_t *_unregisterPollingService;
	static get_current_blocking_context_t *_getCurrentBlockingContext;
	static block_current_task_t *_blockCurrentTask;
	static unblock_task_t *_unblockTask;
	static get_current_event_counter_t *_getCurrentEventCounter;
	static increase_current_task_event_counter_t *_increaseCurrentTaskEventCounter;
	static decrease_task_event_counter_t *_decreaseTaskEventCounter;
	static notify_task_event_counter_api_t *_notifyTaskEventCounterAPI;
	static spawn_function_t *_spawnFunction;
	static wait_for_t *_waitFor;

	static const std::string _registerPollingServiceName;
	static const std::string _unregisterPollingServiceName;
	static const std::string _getCurrentBlockingContextName;
	static const std::string _blockCurrentTaskName;
	static const std::string _unblockTaskName;
	static const std::string _getCurrentEventCounterName;
	static const std::string _increaseCurrentTaskEventCounterName;
	static const std::string _decreaseTaskEventCounterName;
	static const std::string _notifyTaskEventCounterAPIName;
	static const std::string _spawnFunctionName;
	static const std::string _waitForName;

	static EnvironmentVariable<bool> _usePollingServices;
	static EnvironmentVariable<uint64_t> _pollingInterval;

	struct PollingInfo {
		std::string _name;
		polling_function_t _function;
		void *_args;
		std::atomic<bool> _mustFinish;
		std::atomic<bool> _finished;

		inline PollingInfo(const std::string &name, polling_function_t function, void *args) :
			_name(name),
			_function(function),
			_args(args),
			_mustFinish(false),
			_finished(false)
		{
		}
	};

public:
	static void initialize(bool requireTaskBlockingAPI, bool requireTaskEventsAPI);

	static inline polling_handle_t registerPolling(
		const std::string &name,
		polling_function_t function,
		void *data = nullptr
	) {
		PollingInfo *info = new PollingInfo(name, function, data);
		assert(info != nullptr);

		if (_usePollingServices) {
			// Register a polling service
			assert(_registerPollingService);
			(*_registerPollingService)(name.c_str(), pollingServiceFunction, info);
		} else {
			// Spawn a function that will do the periodic polling
			assert(_spawnFunction);
			(*_spawnFunction)(pollingTaskFunction, info, pollingTaskCompletes, info, name.c_str());
		}
		return (polling_handle_t) info;
	}

	static inline void unregisterPolling(polling_handle_t handle)
	{
		PollingInfo *info = (PollingInfo *) handle;
		assert(info != nullptr);

		// Notify that the polling should stop
		info->_mustFinish = true;

		if (_usePollingServices) {
			// Unregister the polling service
			assert(_unregisterPollingService);
			(*_unregisterPollingService)(info->_name.c_str(), pollingServiceFunction, info);
		} else {
			// Wait until the spawned task completes
			while (!info->_finished);
		}
		delete info;
	}

	static inline void *getCurrentBlockingContext()
	{
		assert(_getCurrentBlockingContext);
		return (*_getCurrentBlockingContext)();
	}

	static inline void blockCurrentTask(void *context)
	{
		assert(_blockCurrentTask);
		(*_blockCurrentTask)(context);
	}

	static inline void unblockTask(void *context)
	{
		assert(_unblockTask);
		(*_unblockTask)(context);
	}

	static inline void *getCurrentEventCounter()
	{
		assert(_getCurrentEventCounter);
		return (*_getCurrentEventCounter)();
	}

	static inline void increaseCurrentTaskEventCounter(void *counter, unsigned int increment)
	{
		assert(_increaseCurrentTaskEventCounter);
		(*_increaseCurrentTaskEventCounter)(counter, increment);
	}

	static inline void decreaseTaskEventCounter(void *counter, unsigned int decrement)
	{
		assert(_decreaseTaskEventCounter);
		(*_decreaseTaskEventCounter)(counter, decrement);
	}

private:
	static inline int pollingServiceFunction(void *args)
	{
		PollingInfo *info = (PollingInfo *) args;
		assert(info != nullptr);

		// Call the actual polling function
		info->_function(info->_args);

		// Do not unregister the service
		return 0;
	}

	static inline void pollingTaskFunction(void *args)
	{
		PollingInfo *info = (PollingInfo *) args;
		assert(info != nullptr);
		assert(_waitFor);

		while (!info->_mustFinish) {
			// Call the actual polling function
			info->_function(info->_args);

			// Pause the polling task for some microseconds
			(*_waitFor)(_pollingInterval);
		}
	}

	static inline void pollingTaskCompletes(void *args)
	{
		PollingInfo *info = (PollingInfo *) args;
		assert(info != nullptr);

		// The polling task completed
		info->_finished = true;
	}
};

#endif // TASKING_MODEL_HPP

