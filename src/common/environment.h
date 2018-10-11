/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "allocator.h"
#include "ticket_queue.h"
#include "runtime_api.h"

#include <mpi.h>
#include <atomic>

template <typename Ticket, typename TicketQueue>
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
	
	static bool inSerialContext()
	{
		return nanos6_in_serial_context();
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
		nanos6_register_polling_service("MPI INTEROPERABILITY", GenericEnvironment::poll, nullptr);
		
		Allocator<Ticket>::initialize();
	}
	
	static void finalize()
	{
		nanos6_unregister_polling_service("MPI INTEROPERABILITY", GenericEnvironment::poll, nullptr);
		
		Allocator<Ticket>::finalize();
	}
	
	static TicketQueue &getQueue()
	{
		static TicketQueue _queue;
		return _queue;
	}
};

namespace C {
	typedef GenericEnvironment<Ticket, TicketQueue<Ticket> > Environment;
}

namespace Fortran {
	typedef GenericEnvironment<Ticket, TicketQueue<Ticket> > Environment;
}

#endif // ENVIRONMENT_H
