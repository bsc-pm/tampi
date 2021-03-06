/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2019-2021 Barcelona Supercomputing Center (BSC)
*/

#ifndef TASKING_MODEL_API_HPP
#define TASKING_MODEL_API_HPP

#include <cstdint>


extern "C" {
	/* Polling service API */

	//! \brief Function that the runtime calls periodically
	//!
	//! \param service_data a pointer to data that the function uses and that
	//! also identifies an instance of this service registration
	//!
	//! \returns true to signal that the purpose of the function has been
	//! achieved and that the function should not be called again with the given
	//! service_data
	typedef int (*nanos6_polling_service_t)(void *data);

	//! \brief Register a function and parameter of that function that the runtime
	//! must call periodically to perform operations in a non-blocking fashion
	//!
	//! Registers a function that the runtime will called periodically. The  purpose
	//! is to support operations in a non-blocking way. For instance to check for
	//! certain events that are not possible to check in a blocking way, or that it
	//! is not desirable to do so.
	//!
	//! The service_data parameter is an opaque pointer that is passed to the function
	//! as is.
	//!
	//! The same function can be registered several times with different service_data
	//! parameters, and each combination will be considered as a different
	//! registration. That is, a registered service instance consists of a service
	//! function and a specific service_data.
	//!
	//! The service will remain registered as long as the function returns false and
	//! the service has not been explicitly deregistered through a call to
	//! nanos6_unregister_polling_service.
	//!
	//! \param[in] service_name a string that identifies the kind of service that will
	//! be serviced
	//! \param[in] service the function that the runtime should call periodically
	//! \param service_data an opaque pointer to data that is passed to the service
	//! function
	void nanos6_register_polling_service(char const *name, nanos6_polling_service_t service, void *data);

	//! \brief Unregister a function and parameter of that function previously
	//! registered through a call to nanos6_register_polling_service.
	//!
	//! Unregister a service instance identified by the service_function and
	//! service_data previously registered through a call to
	//! nanos6_register_polling_service. The service_function must not have returned
	//! true when called with the given service_data, since that return value is
	//! equivalent to a call to this function.
	//!
	//! \param[in] service_name a string that identifies the kind of service
	//! \param[in] service the function that the runtime should stop calling
	//! periodically with the given service_data
	//! \param service_data an opaque pointer to the data that was passed to the
	//! service function
	void nanos6_unregister_polling_service(char const *name, nanos6_polling_service_t service, void *data);


	/* Block/Unblock API */

	//! \brief Get a handler that allows to block and unblock the current task
	//!
	//! \returns an opaque pointer that is used for blocking and unblocking
	//! the current task.
	//!
	//! The underlying implementation may or may not return the same value
	//! for repeated calls to this function.
	//!
	//! Once the handler has been used once in a call to nanos6_block_current_task
	//! and a call to nanos6_unblock_task, the handler is discarded and a new
	//! one must be obtained to perform another cycle of blocking and unblocking.
	void *nanos6_get_current_blocking_context(void);

	//! \brief Block the current task
	//!
	//! \param blocking_context the value returned by a call to nanos6_get_current_blocking_context
	//! performed within the task
	//!
	//! This function blocks the execution of the current task at least until
	//! a thread calls nanos6_unblock_task with its blocking context
	//!
	//! The runtime may choose to execute other tasks within the execution scope
	//! of this call.
	void nanos6_block_current_task(void *blocking_context);

	//! \brief Unblock a task previously or about to be blocked
	//!
	//! Mark as unblocked a task previously or about to be blocked inside a call
	//! to nanos6_block_current_task.
	//!
	//! While this function can be called before the actual to nanos6_block_current_task,
	//! only one call to it may precede its matching call to nanos6_block_current_task.
	//!
	//! The return of this function does not guarantee that the blocked task has
	//! resumed yet its execution. It only guarantees that it will be resumed.
	//!
	//! \param[in] blocking_context the handler used to block the task
	void nanos6_unblock_task(void *blocking_context);


	/* External Events API */

	//! \brief Get the event counter associated with the current task
	//!
	//! \returns the associated event counter with the executing task
	void *nanos6_get_current_event_counter(void);

	//! \brief Increase the counter of events of the current task to prevent the release of dependencies
	//!
	//! This function atomically increases the counter of events of a task
	//!
	//! \param[in] event_counter The event counter according with the current task
	//! \param[in] value The value to be incremented (must be positive or zero)
	void nanos6_increase_current_task_event_counter(void *event_counter, unsigned int increment);

	//! \brief Decrease the counter of events of a task and release the dependencies if required
	//!
	//! This function atomically decreases the counter of events of a task and
	//! it releases the depencencies once the number of events becomes zero
	//! and the task has completed its execution
	//!
	//! \param[in] event_counter The event counter of the task
	//! \param[in] value The value to be decremented (must be positive or zero)
	void nanos6_decrease_task_event_counter(void *event_counter, unsigned int decrement);

	//! \brief Notify that the external events API could be used
	//!
	//! This function notifies the runtime system as soon as possible
	//! that the external events API could be used from now on. This
	//! function may be defined or not depending on the runtime system
	void nanos6_notify_task_event_counter_api(void);

	//! \brief Spawn asynchronously a function
	//!
	//! \param function the function to be spawned
	//! \param args a parameter that is passed to the function
	//! \param completion_callback an optional function that will be called when the function finishes
	//! \param completion_args a parameter that is passed to the completion callback
	//! \param label an optional name for the function
	void nanos6_spawn_function(
		void (*function)(void *),
		void *args,
		void (*completion_callback)(void *),
		void *completion_args,
		char const *label);

	//! \brief Pause the current task for an amount of microseconds
	//!
	//! The task is paused for approximately the amount of microseconds
	//! passed as a parameter. The runtime may choose to execute other
	//! tasks within the execution scope of this call
	//!
	//! \param time_us the time that should be spent while paused
	//! in microseconds
	//!
	//! \returns the actual time spent during the pause
	uint64_t nanos6_wait_for(uint64_t time_us);


	/* Instrument API */

	typedef struct {
		int64_t offset_ns;
		uint64_t num_samples;
		double stdev_ns;
	} nanos6_clock_offset_t;

	typedef struct {
		uint64_t rank;
		uint64_t num_ranks;
		nanos6_clock_offset_t clock_offset;
	} nanos6_distributed_instrument_info_t;

	//! \brief Check whether the instrumentation in distributed executions is enabled
	int nanos6_is_distributed_instrument_enabled(void);

	//! \brief Setup the instrumentation in distributed executions
	//!
	//! \param info the information for distributed instrumentation
	void nanos6_setup_distributed_instrument(const nanos6_distributed_instrument_info_t *info);

	//! \brief Get the start time of the instrumentation (ns)
	int64_t nanos6_get_instrument_start_time_ns(void);

	//! Prototypes of the tasking model API functions
	typedef nanos6_polling_service_t polling_service_t;
	typedef void register_polling_service_t(char const *, polling_service_t, void *);
	typedef void unregister_polling_service_t(char const *, polling_service_t, void *);
	typedef void *get_current_blocking_context_t(void);
	typedef void block_current_task_t(void *);
	typedef void unblock_task_t(void *);
	typedef void *get_current_event_counter_t(void);
	typedef void increase_current_task_event_counter_t(void *, unsigned int);
	typedef void decrease_task_event_counter_t(void *, unsigned int);
	typedef void notify_task_event_counter_api_t(void);
	typedef void spawn_function_t(void (*)(void *), void *, void (*)(void *), void *, char const *);
	typedef uint64_t wait_for_t(uint64_t);
	typedef int is_distributed_instrument_enabled_t(void);
	typedef void setup_distributed_instrument_t(const nanos6_distributed_instrument_info_t *);
	typedef int64_t get_instrument_start_time_t(void);
}

#endif // TASKING_MODEL_API_HPP
