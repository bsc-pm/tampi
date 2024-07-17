/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2024 Barcelona Supercomputing Center (BSC)
*/

#include <cstdio>

#include "Environment.hpp"
#include "polling/Polling.hpp"
#include "util/ErrorHandler.hpp"


namespace tampi {

Environment::State Environment::_state;
thread_local bool Environment::State::threadTaskAwareness = true;

TaskingModel::PollingInstance *Polling::_pollingInstance;
TaskingModel::PollingInstance *Polling::_completionPollingInstance;
PollingPeriodCtrl Polling::_periodCtrl("TAMPI_POLLING_PERIOD");
PollingPeriodCtrl Polling::_completionPeriodCtrl("TAMPI_POLLING_TASK_COMPLETION_PERIOD");

EnvironmentVariable<bool> CompletionManager::_enabled("TAMPI_POLLING_TASK_COMPLETION", true);
CompletionManager::queue_t CompletionManager::_queue;

SpinLock ErrorHandler::_lock;

} // namespace tampi

#if !defined(NDEBUG)
namespace boost {
	void assertion_failed_msg(char const * expr, char const * msg, char const * function, char const * file, long line)
	{
		fprintf(stderr, "%s:%ld %s Boost assertion failure: %s when evaluating %s\n", file, line, function, msg, expr);
		abort();
	}

	void assertion_failed(char const * expr, char const * function, char const * file, long line)
	{
		fprintf(stderr, "%s:%ld %s Boost assertion failure when evaluating %s\n", file, line, function, expr);
		abort();
	}
}
#endif
