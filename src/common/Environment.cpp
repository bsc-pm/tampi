/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2019 Barcelona Supercomputing Center (BSC)
*/

#include <cstdio>

#include "Definitions.hpp"
#include "Environment.hpp"
#include "Ticket.hpp"
#include "TicketManager.hpp"
#include "util/ErrorHandler.hpp"


template<>
std::atomic<bool> Environment<C>::_blockingEnabled(false);
template<>
std::atomic<bool> Environment<C>::_nonBlockingEnabled(false);

template<>
std::atomic<bool> Environment<Fortran>::_blockingEnabled(false);
template<>
std::atomic<bool> Environment<Fortran>::_nonBlockingEnabled(false);

C::request_t C::REQUEST_NULL;
C::status_ptr_t C::STATUS_IGNORE;
C::status_ptr_t C::STATUSES_IGNORE;

Fortran::request_t Fortran::REQUEST_NULL;
Fortran::status_ptr_t Fortran::STATUS_IGNORE;
Fortran::status_ptr_t Fortran::STATUSES_IGNORE;

SpinLock ErrorHandler::_lock;

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
