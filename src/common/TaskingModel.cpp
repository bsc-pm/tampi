/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#include "Symbol.hpp"
#include "TaskingModel.hpp"
#include "util/ErrorHandler.hpp"

namespace tampi {

//! Enable/disable polling services
EnvironmentVariable<bool> TaskingModel::_usePollingServices("TAMPI_POLLING_SERVICES", false);


void TaskingModel::initialize(bool requireTaskBlockingAPI, bool requireTaskEventsAPI)
{
	// Find the first occurrence of the desired symbol
	const SymbolAttr attr = SymbolAttr::First;

	if (requireTaskBlockingAPI || requireTaskEventsAPI) {
		// Try to load the functions required by polling tasks
		_waitFor.load(attr, /* mandatory = */ false);
		_spawnFunction.load(attr, /* mandatory = */ false);

		// Switch to polling services if polling tasks are not available
		if (!_waitFor.hasSymbol() || !_spawnFunction.hasSymbol())
			_usePollingServices.set(true);

		// Load the polling service functions if needed
		if (_usePollingServices) {
			_registerPollingService.load(attr);
			_unregisterPollingService.load(attr);
		}
	}

	// Load the task blocking API functions if needed
	if (requireTaskBlockingAPI) {
		_getCurrentBlockingContext.load(attr);
		_blockCurrentTask.load(attr);
		_unblockTask.load(attr);
	}

	// Load the task events API functions if needed
	if (requireTaskEventsAPI) {
		_getCurrentEventCounter.load(attr);
		_increaseCurrentTaskEventCounter.load(attr);
		_decreaseTaskEventCounter.load(attr);

		_notifyTaskEventCounterAPI.load(attr, /* mandatory = */ false);

		// Notify the tasking runtime that the event counters API may be used
		// during the execution. This function is optional; call it if defined
		if (_notifyTaskEventCounterAPI.hasSymbol())
			_notifyTaskEventCounterAPI();
	}
}

Symbol<register_polling_service_t> TaskingModel::_registerPollingService("nanos6_register_polling_service", /* load = */ false);
Symbol<unregister_polling_service_t> TaskingModel::_unregisterPollingService("nanos6_unregister_polling_service", /* load = */ false);
Symbol<get_current_blocking_context_t> TaskingModel::_getCurrentBlockingContext("nanos6_get_current_blocking_context", /* load = */ false);
Symbol<block_current_task_t> TaskingModel::_blockCurrentTask("nanos6_block_current_task", /* load = */ false);
Symbol<unblock_task_t> TaskingModel::_unblockTask("nanos6_unblock_task", /* load = */ false);
Symbol<get_current_event_counter_t> TaskingModel::_getCurrentEventCounter("nanos6_get_current_event_counter", /* load = */ false);
Symbol<increase_current_task_event_counter_t> TaskingModel::_increaseCurrentTaskEventCounter("nanos6_increase_current_task_event_counter", /* load = */ false);
Symbol<decrease_task_event_counter_t> TaskingModel::_decreaseTaskEventCounter("nanos6_decrease_task_event_counter", /* load = */ false);
Symbol<notify_task_event_counter_api_t> TaskingModel::_notifyTaskEventCounterAPI("nanos6_notify_task_event_counter_api", /* load = */ false);
Symbol<spawn_function_t> TaskingModel::_spawnFunction("nanos6_spawn_function", /* load = */ false);
Symbol<wait_for_t> TaskingModel::_waitFor("nanos6_wait_for", /* load = */ false);

} // namespace tampi
