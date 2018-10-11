/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include "allocator.h"
#include "ticket.h"

#include <cstdlib>

template<>
Allocator<C::Ticket>::queue_t* Allocator<C::Ticket>::_queue = nullptr;

template<>
C::Ticket* Allocator<C::Ticket>::_objects = nullptr;

template<>
Allocator<Fortran::Ticket>::queue_t* Allocator<Fortran::Ticket>::_queue = nullptr;

template<>
Fortran::Ticket* Allocator<Fortran::Ticket>::_objects = nullptr;

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
