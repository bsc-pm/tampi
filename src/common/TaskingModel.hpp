/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2019-2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef TASKING_MODEL_HPP
#define TASKING_MODEL_HPP

#include <pthread.h>

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdint>
#include <string>

#include "Symbol.hpp"
#include "TaskingModelAPI.hpp"
#include "util/EnvironmentVariable.hpp"
#include "util/ErrorHandler.hpp"


namespace tampi {

//! Class that gives access to the tasking model features
class TaskingModel {
public:
	//! Prototype of a polling instance function
	typedef uint64_t (*polling_function_t)(void *args, uint64_t waitTimeUs);

	//! Handle of polling instances
	typedef size_t polling_handle_t;

	//! Invalid value for wait time
	static constexpr int64_t INVALID_WAIT_TIME = -1;

private:
	//! Pointers to the tasking model functions
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

	//! Actual names of the tasking model functions
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

	//! Determine whether the polling feature should be done through
	//! polling services. By default it is disabled, which makes the
	//! polling feature to be implemented using tasks that are paused
	//! periodically. This variable is called TAMPI_POLLING_SERVICES
	static EnvironmentVariable<bool> _usePollingServices;

	//! Internal structure to represent a polling instance
	struct PollingInfo {
		std::string _name;
		polling_function_t _function;
		void *_args;
		std::atomic<bool> _mustFinish;
		std::atomic<bool> _finished;

		inline PollingInfo(
			const std::string &name,
			polling_function_t function,
			void *args
		) :
			_name(name),
			_function(function),
			_args(args),
			_mustFinish(false),
			_finished(false)
		{
		}
	};

public:
	//! \brief Initialize and load the symbols of the tasking model
	//!
	//! \param requireTaskBlockingAPI Whether needs the task blocking API
	//! \param requireTaskEventsAPI Whether needs the task events API
	static void initialize(bool requireTaskBlockingAPI, bool requireTaskEventsAPI);

	//! \brief Register a polling instance
	//!
	//! This function registers a polling instance, which will call
	//! the specified function with its argument periodically. The
	//! polling function must accept a pointer to void and should
	//! not return anything
	//!
	//! \param name The name of the polling instance
	//! \param function The function to be called periodically
	//! \param args The arguments of the function
	//!
	//! \returns A polling handle to unregister the instance once
	//!          the polling should finish
	static inline polling_handle_t registerPolling(
		const std::string &name,
		polling_function_t function,
		void *args
	) {
		PollingInfo *info = new PollingInfo(name, function, args);
		assert(info != nullptr);

		if (_usePollingServices) {
			// Register a polling service
			assert(_registerPollingService);
			(*_registerPollingService)(name.c_str(), pollingServiceFunction, info);
		} else {
			// Spawn a function that will do the periodic polling. We spawn
			// the function from an external thread due to a limitation in
			// OmpSs-2 2020.06, which made the spawn function to set the
			// current calling task as its parent. Spawning the function
			// from a thread prevents it from having a parent task
			pthread_t thread;
			int err = pthread_create(&thread, nullptr, spawnPollingTask, info);
			ErrorHandler::failIf(err, "Failed when creating an auxiliary thread\n");

			err = pthread_join(thread, nullptr);
			ErrorHandler::failIf(err, "Failed when joining an auxiliary thread\n");
		}
		return (polling_handle_t) info;
	}

	//! \brief Unregister a polling instance
	//!
	//! This function unregisters a polling instance, which will
	//! prevent the associated polling function from being further
	//! called. The polling function is guaranteed to not be called
	//! after returing from this function. Note that other registered
	//! polling instances may continue calling that function
	//!
	//! \param handle The handle of the polling instance to unregister
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
			assert(_waitFor);

			// Wait until the spawned task completes
			while (!info->_finished) {
				// Yield in case there is a single available CPU
				(*_waitFor)(1000);
			}
		}
		delete info;
	}

	//! \brief Get the blocking context of the current task
	//!
	//! \returns An opaque pointer of the blocking context
	static inline void *getCurrentBlockingContext()
	{
		assert(_getCurrentBlockingContext);
		return (*_getCurrentBlockingContext)();
	}

	//! \brief Block the current task
	//!
	//! \param context The blocking context of the current task
	static inline void blockCurrentTask(void *context)
	{
		assert(_blockCurrentTask);
		(*_blockCurrentTask)(context);
	}

	//! \brief Unblock a task
	//!
	//! \param context The blocking context of the task to unblock
	static inline void unblockTask(void *context)
	{
		assert(_unblockTask);
		(*_unblockTask)(context);
	}

	//! \brief Get the event counter of the current task
	//!
	//! \returns An opaque pointer of the event counter
	static inline void *getCurrentEventCounter()
	{
		assert(_getCurrentEventCounter);
		return (*_getCurrentEventCounter)();
	}

	//! \brief Increase the event counter of the current task
	//!
	//! \param counter The event counter of the current task
	//! \param increment The amount of events to increase
	static inline void increaseCurrentTaskEventCounter(void *counter, unsigned int increment)
	{
		assert(_increaseCurrentTaskEventCounter);
		(*_increaseCurrentTaskEventCounter)(counter, increment);
	}

	//! \brief Decrease the event counter of a task
	//!
	//! \param counter The event counter of the target task
	//! \param decrement The amount of events to decrease
	static inline void decreaseTaskEventCounter(void *counter, unsigned int decrement)
	{
		assert(_decreaseTaskEventCounter);
		(*_decreaseTaskEventCounter)(counter, decrement);
	}

private:
	//! \brief Function called by all polling services
	//!
	//! This function is periodically called by all polling
	//! services when implementing polling instances with
	//! that tasking model feature
	//!
	//! \param args An opaque pointer to the polling info
	//!
	//! \returns Whether the polling service should unregister
	static inline int pollingServiceFunction(void *args)
	{
		PollingInfo *info = (PollingInfo *) args;
		assert(info != nullptr);

		// Call the actual polling function
		info->_function(info->_args, 0);

		// Do not unregister the service
		return 0;
	}

	//! \brief Function called by all polling tasks
	//!
	//! This is called only once per polling task when
	//! implementing polling instances with that tasking
	//! model feature. This represents the body of all
	//! polling tasks
	//!
	//! \param args An opaque pointer to the polling info
	static inline void pollingTaskFunction(void *args)
	{
		PollingInfo *info = (PollingInfo *) args;
		assert(info != nullptr);
		assert(_waitFor);

		uint64_t waitTimeUs = std::numeric_limits<uint64_t>::max();

		// Poll until it is externally notified to stop
		while (!info->_mustFinish) {
			// Call the actual polling function
			waitTimeUs = info->_function(info->_args, waitTimeUs);

			// Pause the polling task for some microseconds
			waitTimeUs = (*_waitFor)(waitTimeUs);
		}
	}

	//! \brief Spawn a polling task
	//!
	//! \param args An opaque pointer to the polling info
	static inline void *spawnPollingTask(void *args)
	{
		PollingInfo *info = (PollingInfo *) args;
		assert(info != nullptr);

		assert(_spawnFunction);
		(*_spawnFunction)(pollingTaskFunction, info, pollingTaskCompletes, info, info->_name.c_str());

		return nullptr;
	}

	//! \brief Function called by a polling task when completes
	//!
	//! This function is called when a polling task fully
	//! completes (e.g., all child tasks have completed)
	//!
	//! \param args An opaque pointer to the polling info
	static inline void pollingTaskCompletes(void *args)
	{
		PollingInfo *info = (PollingInfo *) args;
		assert(info != nullptr);

		// The polling task has completed
		info->_finished = true;
	}
};

} // namespace tampi

#endif // TASKING_MODEL_HPP

