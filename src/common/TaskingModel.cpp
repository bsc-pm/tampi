/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2025 Barcelona Supercomputing Center (BSC)
*/

#include "Symbol.hpp"
#include "TaskingModel.hpp"
#include "util/ErrorHandler.hpp"

namespace tampi {

void (*TaskingModel::pollingFunction)(void *args);

void TaskingModel::initialize(bool requireTaskBlockingAPI, bool requireTaskEventsAPI)
{
	// Avoid loading symbols if no mode is required
	if (!requireTaskBlockingAPI && !requireTaskEventsAPI)
		return;

	// Find the first occurrence of the desired symbol
	const SymbolAttr attr = SymbolAttr::First;

	_alpi_error_string.load(attr);
	_alpi_info_get.load(attr, false); // ALPI 1.2
	_alpi_feature_check.load(attr, false); // ALPI 1.2
	_alpi_version_check.load(attr);
	_alpi_version_get.load(attr);
	_alpi_task_self.load(attr);

	// Load the task blocking API functions if needed
	if (requireTaskBlockingAPI) {
		_alpi_task_block.load(attr);
		_alpi_task_unblock.load(attr);
	}

	_alpi_task_waitfor_ns.load(attr);

	// Load the task events API functions if needed
	if (requireTaskEventsAPI) {
		_alpi_task_events_increase.load(attr);
		_alpi_task_events_test.load(attr, false); // ALPI 1.1
		_alpi_task_events_decrease.load(attr);
	}

	_alpi_task_spawn.load(attr);

	_alpi_cpu_count.load(attr);
	_alpi_cpu_logical_id.load(attr);
	_alpi_cpu_system_id.load(attr);

	_alpi_task_suspend.load(attr, false); // ALPI 1.1
	_alpi_task_suspend_mode_set.load(attr, false); // ALPI 1.1

	// Keep compatibility with major version
	int expected[2] = { ALPI_VERSION_MAJOR, false /* ALPI_VERSION_MINOR */ };
	int provided[2] = { false, false };

	_alpi_version_get(&provided[0], &provided[1]);

	int err = _alpi_version_check.callNative(expected[0], expected[1]);
	if (err == ALPI_ERR_VERSION)
		ErrorHandler::fail("Incompatible ALPI tasking interface versions:\n",
			"\tExpected: ", expected[0], ".", expected[1], "\n",
			"\tProvided: ", provided[0], ".", provided[1]);
	else if (err)
		_alpi_version_check.processFailure(err);

	const bool featureCheckAvailable =
		provided[0] > 1 || (provided[0] == 1 && provided[1] >= 2); // >= 1.2

	bool useSuspendPolling = featureCheckAvailable &&
		ALPI_SUCCESS == _alpi_feature_check.callNative(ALPI_FEATURE_SUSPEND);

	const std::string pollingMode = _pollingMode.get();
	if (pollingMode == "suspend") {
		ErrorHandler::failIf(!useSuspendPolling,
			"Use of suspend polling API was forced, but the "
			"ALPI implementor does not support it");
	} else if (pollingMode == "blocking") {
		ErrorHandler::warnIf(useSuspendPolling,
			"Using blocking polling mode instead of suspend");
		useSuspendPolling = false;
	} else if (pollingMode == "auto") {
		ErrorHandler::warnIf(!useSuspendPolling,
			"ALPI suspend not supported by tasking model, "
			"Using blocking polling implementation");
	} else {
		ErrorHandler::fail(
			"Unknown polling mode requested: ", pollingMode,
			"\n\tExpected one of: suspend, blocking or auto.\n");
	}

	pollingFunction = useSuspendPolling? suspendPolling : genericPolling;
}

ALPISymbol<ALPISymbolDecl::alpi_error_string_t>
TaskingModel::_alpi_error_string("alpi_error_string", nullptr);

ALPISymbol<ALPISymbolDecl::alpi_info_get_t>
TaskingModel::_alpi_info_get("alpi_info_get", &_alpi_error_string);

ALPISymbol<ALPISymbolDecl::alpi_feature_check_t>
TaskingModel::_alpi_feature_check("alpi_feature_check", &_alpi_error_string);

ALPISymbol<ALPISymbolDecl::alpi_version_check_t>
TaskingModel::_alpi_version_check("alpi_version_check", &_alpi_error_string);

ALPISymbol<ALPISymbolDecl::alpi_version_get_t>
TaskingModel::_alpi_version_get("alpi_version_get", &_alpi_error_string);

ALPISymbol<ALPISymbolDecl::alpi_task_self_t>
TaskingModel::_alpi_task_self("alpi_task_self", &_alpi_error_string);

ALPISymbol<ALPISymbolDecl::alpi_task_block_t>
TaskingModel::_alpi_task_block("alpi_task_block", &_alpi_error_string);

ALPISymbol<ALPISymbolDecl::alpi_task_unblock_t>
TaskingModel::_alpi_task_unblock("alpi_task_unblock", &_alpi_error_string);

ALPISymbol<ALPISymbolDecl::alpi_task_waitfor_ns_t>
TaskingModel::_alpi_task_waitfor_ns("alpi_task_waitfor_ns", &_alpi_error_string);

ALPISymbol<ALPISymbolDecl::alpi_task_events_increase_t>
TaskingModel::_alpi_task_events_increase("alpi_task_events_increase", &_alpi_error_string);

ALPISymbol<ALPISymbolDecl::alpi_task_events_test_t>
TaskingModel::_alpi_task_events_test("alpi_task_events_test", &_alpi_error_string);

ALPISymbol<ALPISymbolDecl::alpi_task_events_decrease_t>
TaskingModel::_alpi_task_events_decrease("alpi_task_events_decrease", &_alpi_error_string);

ALPISymbol<ALPISymbolDecl::alpi_task_spawn_t>
TaskingModel::_alpi_task_spawn("alpi_task_spawn", &_alpi_error_string);

ALPISymbol<ALPISymbolDecl::alpi_cpu_count_t>
TaskingModel::_alpi_cpu_count("alpi_cpu_count", &_alpi_error_string);

ALPISymbol<ALPISymbolDecl::alpi_cpu_logical_id_t>
TaskingModel::_alpi_cpu_logical_id("alpi_cpu_logical_id", &_alpi_error_string);

ALPISymbol<ALPISymbolDecl::alpi_cpu_system_id_t>
TaskingModel::_alpi_cpu_system_id("alpi_cpu_system_id", &_alpi_error_string);

ALPISymbol<ALPISymbolDecl::alpi_task_suspend_mode_set_t>
TaskingModel::_alpi_task_suspend_mode_set("alpi_task_suspend_mode_set", &_alpi_error_string);

ALPISymbol<ALPISymbolDecl::alpi_task_suspend_t>
TaskingModel::_alpi_task_suspend("alpi_task_suspend", &_alpi_error_string);

} // namespace tampi
