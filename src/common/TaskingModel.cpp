/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#include "Symbol.hpp"
#include "TaskingModel.hpp"
#include "util/ErrorHandler.hpp"

namespace tampi {

void TaskingModel::initialize(bool requireTaskBlockingAPI, bool requireTaskEventsAPI)
{
	// Avoid loading symbols if no mode is required
	if (!requireTaskBlockingAPI && !requireTaskEventsAPI)
		return;

	// Find the first occurrence of the desired symbol
	const SymbolAttr attr = SymbolAttr::First;

	_alpi_version_check.load(attr);
	_alpi_version_get.load(attr);
	_alpi_task_self.load(attr);
	_alpi_task_spawn.load(attr);
	_alpi_task_waitfor_ns.load(attr);

	// Load the task blocking API functions if needed
	if (requireTaskBlockingAPI) {
		_alpi_task_block.load(attr);
		_alpi_task_unblock.load(attr);
	}

	// Load the task events API functions if needed
	if (requireTaskEventsAPI) {
		_alpi_task_events_increase.load(attr);
		_alpi_task_events_decrease.load(attr);
	}

	int expected[2] = { ALPI_VERSION_MAJOR, ALPI_VERSION_MINOR };
	int provided[2];

	if (int err = _alpi_version_get(&provided[0], &provided[1]))
		ErrorHandler::fail("Failed alpi_version_get: ", getError(err));

	int err = _alpi_version_check(expected[0], expected[1]);
	if (err == ALPI_ERR_VERSION)
		ErrorHandler::fail("Incompatible ALPI tasking interface versions:\n",
			"\tExpected: ", expected[0], ".", expected[1], "\n",
			"\tProvided: ", provided[0], ".", provided[1]);
	else if (err)
		ErrorHandler::fail("Failed alpi_version_check: ", getError(err));
}

Symbol<TaskingModel::alpi_error_string_t>
TaskingModel::_alpi_error_string("alpi_error_string", /* load */ false);

Symbol<TaskingModel::alpi_version_check_t>
TaskingModel::_alpi_version_check("alpi_version_check", /* load */ false);

Symbol<TaskingModel::alpi_version_get_t>
TaskingModel::_alpi_version_get("alpi_version_get", /* load */ false);

Symbol<TaskingModel::alpi_task_self_t>
TaskingModel::_alpi_task_self("alpi_task_self", /* load */ false);

Symbol<TaskingModel::alpi_task_block_t>
TaskingModel::_alpi_task_block("alpi_task_block", /* load */ false);

Symbol<TaskingModel::alpi_task_unblock_t>
TaskingModel::_alpi_task_unblock("alpi_task_unblock", /* load */ false);

Symbol<TaskingModel::alpi_task_events_increase_t>
TaskingModel::_alpi_task_events_increase("alpi_task_events_increase", /* load */ false);

Symbol<TaskingModel::alpi_task_events_decrease_t>
TaskingModel::_alpi_task_events_decrease("alpi_task_events_decrease", /* load */ false);

Symbol<TaskingModel::alpi_task_waitfor_ns_t>
TaskingModel::_alpi_task_waitfor_ns("alpi_task_waitfor_ns", /* load */ false);

Symbol<TaskingModel::alpi_task_spawn_t>
TaskingModel::_alpi_task_spawn("alpi_task_spawn", /* load */ false);

} // namespace tampi
