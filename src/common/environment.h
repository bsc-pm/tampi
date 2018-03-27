/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "ticket_queue.h"

#ifdef HAVE_NANOX_NANOS_H
#include <nanox/nanos.h>
#endif

#ifdef HAVE_NANOS6_H
#include <nanos6.h>
#endif

#include <mpi.h>
#include <atomic>

template < typename Queue >
class GenericEnvironment {
private:
	static std::atomic<bool> _enabled;
	
	static int poll(__attribute__((unused)) void *data)
	{
		getQueue().poll();
		return 0;
	}
	
public:
	GenericEnvironment() = delete;
	
	GenericEnvironment(const GenericEnvironment &) = delete;
	
	const GenericEnvironment& operator= (const GenericEnvironment &) = delete;
	
	static bool isEnabled()
	{
		return _enabled.load();
	}
	
	static void enable()
	{
		_enabled.store(true);
	}
	
	static void disable()
	{
		_enabled.store(false);
	}
	
	static void initialize()
	{
		nanos_register_polling_service("MPI INTEROPERABILITY", GenericEnvironment::poll, nullptr);
	}
	
	static void finalize()
	{
		nanos_unregister_polling_service("MPI INTEROPERABILITY", GenericEnvironment::poll, nullptr);
	}
	
	static Queue& getQueue()
	{
		static Queue _queue;
		return _queue;
	}
};

namespace C {
	typedef GenericEnvironment<TicketQueue<C::Ticket> > Environment;
}

namespace Fortran {
	typedef GenericEnvironment<TicketQueue<Fortran::Ticket> > Environment;
}

#endif // ENVIRONMENT_H
