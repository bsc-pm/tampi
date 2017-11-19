/*
 * Copyright (c) 2017 Barcelona Supercomputing Center. All rights
 *                    reserved.
 *
 * This file is part of OmpSs-MPI interoperability library.
 *
 * OmpSs-MPI interop. library is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
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

template < typename Queue >
class GenericEnvironment {
private:
	static bool _enabled;
	
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
		return _enabled;
	}
	
	static void enable()
	{
		_enabled = true;
	}
	
	static void disable()
	{
		_enabled = false;
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
