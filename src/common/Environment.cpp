/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#include <cstdio>

#include "Environment.hpp"
#include "Polling.hpp"
#include "Ticket.hpp"
#include "TicketManager.hpp"
#include "util/ErrorHandler.hpp"


namespace tampi {

Environment::State Environment::_state;

TaskingModel::polling_handle_t Polling::_pollingHandle;
PollingPeriodCtrl Polling::_periodCtrl;

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
