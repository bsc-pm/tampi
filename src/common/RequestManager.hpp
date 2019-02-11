/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef REQUEST_MANAGER_HPP
#define REQUEST_MANAGER_HPP

#include "util/Error.hpp"

#include "Environment.hpp"
#include "Ticket.hpp"
#include "TicketManager.hpp"

#include <mpi.h>
#include <cassert>

template <typename Lang>
class RequestManager {
private:
	typedef typename Lang::request_t request_t;
	typedef typename Lang::status_t  status_t;
	typedef typename Lang::status_ptr_t status_ptr_t;
	typedef ::TicketManager<Lang> TicketManager;
	typedef ::Ticket<Lang> Ticket;
	
public:
	static void processRequest(request_t &request, status_ptr_t status = Lang::STATUS_IGNORE, bool blocking = true);
	
	static void processRequests(util::array_view<request_t> requests, status_ptr_t statuses = Lang::STATUSES_IGNORE, bool blocking = true);
};

template <>
inline void RequestManager<C>::processRequest(request_t &request, status_ptr_t status, bool blocking)
{
	int err;
	int finished = 0;
	err = PMPI_Test(&request, &finished, status);
	error::failIf(err != MPI_SUCCESS, "Unexpected return code from MPI_Test");
	
	if (!finished) {
		TicketManager &manager = Environment<C>::getTicketManager();
		
		if (blocking) {
			Ticket ticket(status);
			manager.addBlockingRequest(request, ticket);
			ticket.wait();
		} else {
			manager.addNonBlockingRequest(request, status);
		}
	}
}

template <>
inline void RequestManager<C>::processRequests(util::array_view<request_t> requests, status_ptr_t statuses, bool blocking)
{
	assert(!requests.empty());
	
	int err;
	int finished = 0;
	err = PMPI_Testall(requests.size(), requests.begin(), &finished, statuses);
	error::failIf(err != MPI_SUCCESS, "Unexpected return code from MPI_Testall");
	
	if (!finished) {
		TicketManager &manager = Environment<C>::getTicketManager();
		
		if (blocking) {
			Ticket ticket(statuses);
			manager.addBlockingRequests(requests, ticket);
			ticket.wait();
		} else {
			manager.addNonBlockingRequests(requests, statuses);
		}
	}
}

template <>
inline void RequestManager<Fortran>::processRequest(request_t &request, status_ptr_t status, bool blocking)
{
	int err;
	int finished = 0;
	pmpi_test_(&request, &finished, status, &err);
	error::failIf(err != MPI_SUCCESS, "Unexpected return code from MPI_Test");
	
	if (!finished) {
		TicketManager &manager = Environment<Fortran>::getTicketManager();
		
		if (blocking) {
			Ticket ticket(status);
			manager.addBlockingRequest(request, ticket);
			ticket.wait();
		} else {
			manager.addNonBlockingRequest(request, status);
		}
	}
}

template <>
inline void RequestManager<Fortran>::processRequests(util::array_view<request_t> requests, status_ptr_t statuses, bool blocking)
{
	assert(!requests.empty());
	
	int err;
	int finished = 0;
	int size = requests.size();
	pmpi_testall_(&size, requests.begin(), &finished, statuses, &err);
	error::failIf(err != MPI_SUCCESS, "Unexpected return code from MPI_Testall");
	
	if (!finished) {
		TicketManager &manager = Environment<Fortran>::getTicketManager();
		
		if (blocking) {
			Ticket ticket(statuses);
			manager.addBlockingRequests(requests, ticket);
			ticket.wait();
		} else {
			manager.addNonBlockingRequests(requests, statuses);
		}
	}
}

#endif // REQUEST_MANAGER_HPP
