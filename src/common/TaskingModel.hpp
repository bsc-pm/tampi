/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2019-2025 Barcelona Supercomputing Center (BSC)
*/

#ifndef TASKING_MODEL_HPP
#define TASKING_MODEL_HPP

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>

#include "ALPI.hpp"
#include "Symbol.hpp"
#include "util/EnvironmentVariable.hpp"

namespace tampi {

//! The symbol declarations of the ALPI functions
struct ALPISymbolDecl {
	using alpi_error_string_t = SymbolDecl<const char *, int>;
	using alpi_info_get_t = SymbolDecl<int, alpi_info_t, char *, size_t, int *>;
	using alpi_feature_check_t = SymbolDecl<int, int>;
	using alpi_version_check_t = SymbolDecl<int, int, int>;
	using alpi_version_get_t = SymbolDecl<int, int *, int *>;

	using alpi_task_self_t = SymbolDecl<int, struct alpi_task **>;

	using alpi_task_block_t = SymbolDecl<int, struct alpi_task *>;
	using alpi_task_unblock_t = SymbolDecl<int, struct alpi_task *>;
	using alpi_task_waitfor_ns_t = SymbolDecl<int, uint64_t, uint64_t *>;

	using alpi_task_events_increase_t = SymbolDecl<int, struct alpi_task *, uint64_t>;
	using alpi_task_events_test_t = SymbolDecl<int, struct alpi_task *, uint64_t *>;
	using alpi_task_events_decrease_t = SymbolDecl<int, struct alpi_task *, uint64_t>;

	using alpi_task_spawn_t = SymbolDecl<int, void (*)(void *), void *, void (*)(void *), void *, const char *, const struct alpi_attr *>;

	using alpi_cpu_count_t = SymbolDecl<int, uint64_t *>;
	using alpi_cpu_logical_id_t = SymbolDecl<int, uint64_t *>;
	using alpi_cpu_system_id_t = SymbolDecl<int, uint64_t *>;

	using alpi_task_suspend_mode_set_t = SymbolDecl<int, struct alpi_task *, alpi_suspend_mode_t, uint64_t>;
	using alpi_task_suspend_t = SymbolDecl<int, struct alpi_task *>;
};

template <typename Decl>
class ALPISymbol : public Symbol<Decl> {
public:

private:
	//! The symbol base type
	using BaseSymbol = Symbol<Decl>;

	//! The symbol type for retrieving error descriptions
	using ErrorSymbol = ALPISymbol<ALPISymbolDecl::alpi_error_string_t>;

	//! The symbol for retrieving error descriptions
	const ErrorSymbol *_errorSymbol;

public:
	ALPISymbol(std::string_view name, const ErrorSymbol *errorSymbol) :
		BaseSymbol(name, false),
		_errorSymbol(errorSymbol)
	{
	}

	//! \brief Execute the function and check result
	template <typename... Params>
	void operator()(Params &&... params) const
	{
		int err = BaseSymbol::operator()(std::forward<Params>(params)...);
		if (err)
			processFailure(err);
	}

	//! \brief Execute the function and check result
	template <typename... Params>
	void call(Params &&... params) const
	{
		int err = BaseSymbol::operator()(std::forward<Params>(params)...);
		if (err)
			processFailure(err);
	}

	//! \brief Execute the function without checking
	template <typename... Params>
	typename BaseSymbol::ReturnTy callNative(Params &&... params) const
	{
		return BaseSymbol::operator()(std::forward<Params>(params)...);
	}

	//! \brief Process failure on a symbol call
	//!
	//! \param error The error code
	void processFailure(int error) const
	{
		const char *string = "No description";
		if (_errorSymbol != nullptr)
			string = _errorSymbol->callNative(error);

		ErrorHandler::fail("Failed ", this->_name, ": ", string);
	}
};

//! Class that gives access to the tasking model features
class TaskingModel {
public:
	typedef struct alpi_task *task_handle_t;
	typedef uint64_t (*polling_function_t)(void *args);

	//! Structure that stores information regarding a polling instance
	struct PollingInstance {
		std::string _name;
		polling_function_t _function;
		void *_args;
		std::atomic<bool> _mustFinish;
		std::atomic<bool> _finished;

		PollingInstance(const std::string &name, polling_function_t function, void *args) :
			_name(name), _function(function), _args(args),
			_mustFinish(false), _finished(false)
		{
		}
	};

private:
	//! The symbols of the tasking model functions
	static ALPISymbol<ALPISymbolDecl::alpi_error_string_t> _alpi_error_string;
	static ALPISymbol<ALPISymbolDecl::alpi_info_get_t> _alpi_info_get;
	static ALPISymbol<ALPISymbolDecl::alpi_feature_check_t> _alpi_feature_check;
	static ALPISymbol<ALPISymbolDecl::alpi_version_check_t> _alpi_version_check;
	static ALPISymbol<ALPISymbolDecl::alpi_version_get_t> _alpi_version_get;

	static ALPISymbol<ALPISymbolDecl::alpi_task_self_t> _alpi_task_self;

	static ALPISymbol<ALPISymbolDecl::alpi_task_block_t> _alpi_task_block;
	static ALPISymbol<ALPISymbolDecl::alpi_task_unblock_t> _alpi_task_unblock;
	static ALPISymbol<ALPISymbolDecl::alpi_task_waitfor_ns_t> _alpi_task_waitfor_ns;

	static ALPISymbol<ALPISymbolDecl::alpi_task_events_increase_t> _alpi_task_events_increase;
	static ALPISymbol<ALPISymbolDecl::alpi_task_events_test_t> _alpi_task_events_test;
	static ALPISymbol<ALPISymbolDecl::alpi_task_events_decrease_t> _alpi_task_events_decrease;

	static ALPISymbol<ALPISymbolDecl::alpi_task_spawn_t> _alpi_task_spawn;

	static ALPISymbol<ALPISymbolDecl::alpi_cpu_count_t> _alpi_cpu_count;
	static ALPISymbol<ALPISymbolDecl::alpi_cpu_logical_id_t> _alpi_cpu_logical_id;
	static ALPISymbol<ALPISymbolDecl::alpi_cpu_system_id_t> _alpi_cpu_system_id;

	static ALPISymbol<ALPISymbolDecl::alpi_task_suspend_mode_set_t> _alpi_task_suspend_mode_set;
	static ALPISymbol<ALPISymbolDecl::alpi_task_suspend_t> _alpi_task_suspend;

	static EnvironmentVariable<std::string> _pollingMode;
	static void (*pollingFunction)(void *args);

public:
	//! \brief Initialize and load the symbols of the tasking model
	//!
	//! \param requireTaskBlockingAPI Whether needs the task blocking API
	//! \param requireTaskEventsAPI Whether needs the task events API
	static void initialize(bool requireTaskBlockingAPI, bool requireTaskEventsAPI);

	//! \brief Register a polling instance
	//!
	//! This function registers a polling instance, which will call the
	//! specified function with its argument periodically. The polling
	//! function must follow the type polling_function_t
	//!
	//! \param name The name of the polling instance
	//! \param function The function to be called periodically
	//! \param args The arguments of the function
	//!
	//! \returns A polling handle to unregister the instance once
	//!          the polling should finish
	static PollingInstance *registerPolling(
		const std::string &name,
		polling_function_t function,
		void *args
	) {
		PollingInstance *instance = new PollingInstance(name, function, args);
		assert(instance != nullptr);

		// Spawn a task that will do the periodic polling
		_alpi_task_spawn(
			pollingFunction,
			static_cast<void *>(instance),
			genericCompleted, static_cast<void *>(instance),
			name.data(), nullptr);

		return instance;
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
	static void unregisterPolling(PollingInstance *instance)
	{
		assert(instance != nullptr);

		// Notify that the polling should stop
		instance->_mustFinish = true;

		// Wait until the spawned task completes
		while (!instance->_finished) {
			// Task yield to avoid consuming a CPU for waiting. Otherwise, in
			// the case of a single CPU, the execution could hang
			_alpi_task_waitfor_ns(1000000, nullptr);
		}
		delete instance;
	}

	//! \brief Get the current task handle
	static task_handle_t getCurrentTask()
	{
		struct alpi_task *task;
		_alpi_task_self(&task);

		return task;
	}

	//! \brief Block the current task
	//!
	//! \param task The current task's handle
	static void blockCurrentTask(task_handle_t task)
	{
		_alpi_task_block(task);
	}

	//! \brief Unblock a task
	//!
	//! \param task The task's handle to unblock
	static void unblockTask(task_handle_t task)
	{
		_alpi_task_unblock(task);
	}

	//! \brief Increase the events of the current task
	//!
	//! \param task The current task's handle
	//! \param increment The amount of events to increase
	static void increaseCurrentTaskEvents(task_handle_t task, uint64_t increment)
	{
		_alpi_task_events_increase(task, increment);
	}

	//! \brief Decrease the events of a task
	//!
	//! \param task The task's handle to decrease events
	//! \param decrement The amount of events to decrease
	static void decreaseTaskEvents(task_handle_t task, uint64_t decrement)
	{
		_alpi_task_events_decrease(task, decrement);
	}

	//! \brief Get the number of logical available CPUs
	static uint64_t getNumLogicalCPUs()
	{
		uint64_t count;
		_alpi_cpu_count(&count);
		return count;
	}

	//! \brief Get the virtual CPU id where the current thread is running
	static uint64_t getCurrentLogicalCPU()
	{
		uint64_t id;
		_alpi_cpu_logical_id(&id);
		return id;
	}

	//! \brief Get the system CPU id where the current thread is running
	static uint64_t getCurrentSystemCPU()
	{
		uint64_t id;
		_alpi_cpu_system_id(&id);
		return id;
	}

	//! \brief Suspend current task with timeout
	static void suspendCurrentWithTimeout(uint64_t timeout_ns)
	{
		task_handle_t task = getCurrentTask();
		if (timeout_ns == 0) {
			/** Submit itself, if args is 0 will do a normal submit, if args is 1 the task will yield */
			_alpi_task_suspend_mode_set(task, ALPI_SUSPEND_SUBMIT, 0);
		} else {
			_alpi_task_suspend_mode_set(task, ALPI_SUSPEND_TIMEOUT_SUBMIT, timeout_ns);
		}
		_alpi_task_suspend(task);
	}

private:
	//! \brief Wrapper function called by all polling tasks
	//!
	//! This function is called once for each polling instance and is
	//! executed by a task. This function runs on a loop until the
	//! instance is unregistered. The body of the loop performs a call
	//! to the polling instance function and then blocks the task for
	//! a time specified by that function as its return value
	//!
	//! \param args An opaque pointer to the polling instance
	//
	//! \note If alpi suspend API is available, `suspendPolling` is used
	//! instead
	static void genericPolling(void *args)
	{
		PollingInstance *instance = static_cast<PollingInstance *>(args);
		assert(instance != nullptr);

		uint64_t target, last = 0;

		// Poll until it is externally notified to stop
		while (!instance->_mustFinish) {
			// Call the actual polling function
			target = instance->_function(instance->_args);

			// Pause the polling task for some microseconds
			_alpi_task_waitfor_ns(target * 1000, &last);

			last = last / 1000;
		}
	}

	//! \brief Wrapper function called by all polling tasks when alpi suspend
	//! API is available.
	//!
	//! \param args An opaque pointer to the polling instance
	static void suspendPolling(void *args)
	{
		PollingInstance *instance = static_cast<PollingInstance *>(args);
		assert(instance != nullptr);

		uint64_t target;

		// Poll until it is externally notified to stop
		if (instance->_mustFinish)
			return;

		// Call the actual polling function
		target = instance->_function(instance->_args);

		// Suspend the polling task for some microseconds
		suspendCurrentWithTimeout(target * 1000);
	}

	//! \brief Function called by a polling task is completed
	//!
	//! \param args An opaque pointer to the polling instance
	static void genericCompleted(void *args)
	{
		PollingInstance *instance = static_cast<PollingInstance *>(args);
		assert(instance != nullptr);

		// The polling task has completed
		instance->_finished = true;
	}
};

} // namespace tampi

#endif // TASKING_MODEL_HPP
