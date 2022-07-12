/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2022 Barcelona Supercomputing Center (BSC)
*/

#include <cstdio>

#include "Definitions.hpp"
#include "Environment.hpp"
#include "Instrument.hpp"
#include "Ticket.hpp"
#include "TicketManager.hpp"
#include "util/ErrorHandler.hpp"


namespace tampi {

std::atomic<bool> Environment::_blockingEnabled(false);
std::atomic<bool> Environment::_nonBlockingEnabled(false);
TaskingModel::polling_handle_t Environment::_pollingHandle;

C::request_t C::REQUEST_NULL;
C::status_ptr_t C::STATUS_IGNORE;
C::status_ptr_t C::STATUSES_IGNORE;

Fortran::request_t Fortran::REQUEST_NULL;
Fortran::status_ptr_t Fortran::STATUS_IGNORE;
Fortran::status_ptr_t Fortran::STATUSES_IGNORE;

EnvironmentVariable<bool> Instrument::_instrumentEnabled("TAMPI_INSTRUMENT", true);
EnvironmentVariable<size_t> Instrument::_instrumentSyncIterations("TAMPI_INSTRUMENT_SYNC_ITERATIONS", 10);

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
