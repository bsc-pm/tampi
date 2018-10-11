/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef PROCESS_REQUEST_H
#define PROCESS_REQUEST_H

#include "allocator.h"
#include "environment.h"
#include "ticket.h"
#include "ticket_queue.h"

#include <mpi.h>
#include <cassert>

namespace C {
	static inline void processRequest(Ticket::Request &request, Ticket::Status *status = MPI_STATUSES_IGNORE, bool blockable = true)
	{
		int finished = 0;
		PMPI_Test(&request, &finished, status);
		
		if (!finished) {
			TicketQueue<Ticket> &queue = Environment::getQueue();
			
			if (blockable) {
				Ticket ticket(request, status, true);
				queue.add(ticket, request);
				ticket.wait();
			} else {
				Ticket *ticket = Allocator<Ticket>::allocate(request, status, false);
				assert(ticket != nullptr);
				queue.add(*ticket, request);
			}
		}
	}
	
	static inline void processRequests(util::array_view<Ticket::Request> requests, Ticket::Status *statuses = MPI_STATUSES_IGNORE, bool blockable = true)
	{
		assert(!requests.empty());
		
		int finished = 0;
		PMPI_Testall(requests.size(), requests.begin(), &finished, statuses);
		
		if (!finished) {
			TicketQueue<Ticket> &queue = Environment::getQueue();
			
			if (blockable) {
				Ticket ticket(requests, statuses, true);
				queue.add(ticket, requests);
				ticket.wait();
			} else {
				Ticket *ticket = Allocator<Ticket>::allocate(requests, statuses, false);
				assert(ticket != nullptr);
				queue.add(*ticket, requests);
			}
		}
	}
} // namespace C


namespace Fortran {
	static inline void processRequest(Fortran::Ticket::Request &request, MPI_Fint *status = MPI_F_STATUS_IGNORE, bool blockable = true)
	{
		int finished = 0;
		int err = MPI_SUCCESS;
		pmpi_test_(&request, &finished, status, &err);
		assert(!err);
		
		if (!finished) {
			TicketQueue<Ticket> &queue = Environment::getQueue();
			
			if (blockable) {
				Ticket ticket(request, status, true);
				queue.add(ticket, request);
				ticket.wait();
			} else {
				Ticket *ticket = Allocator<Ticket>::allocate(request, status, false);
				assert(ticket != nullptr);
				queue.add(*ticket, request);
			}
		}
	}
	
	static inline void processRequests(util::array_view<Ticket::Request> requests, MPI_Fint *statuses = MPI_F_STATUSES_IGNORE, bool blockable = true)
	{
		assert(!requests.empty());
		
		int finished = 0;
		int err = MPI_SUCCESS;
		int size = requests.size();
		pmpi_testall_(&size, requests.begin(), &finished, statuses, &err);
		
		if (!finished) {
			TicketQueue<Ticket> &queue = Environment::getQueue();
			
			if (blockable) {
				Ticket ticket(requests, statuses, true);
				queue.add(ticket, requests);
				ticket.wait();
			} else {
				Ticket *ticket = Allocator<Ticket>::allocate(requests, statuses, false);
				assert(ticket != nullptr);
				queue.add(*ticket, requests);
			}
		}
	}
} // namespace Fortran

#endif // PROCESS_REQUEST_H
