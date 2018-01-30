#ifndef PROCESS_REQUEST_H
#define PROCESS_REQUEST_H

#include "environment.h"
#include "ticket.h"
#include "ticket_queue.h"

#include <mpi.h>
#include <cassert>

namespace C {
	static inline void processRequest(Ticket::Request &request, Ticket::Status *status = MPI_STATUS_IGNORE)
	{
		int finished = 0;
		PMPI_Test(&request, &finished, status);
		
		if (!finished) {
			Ticket ticket(request, status);
			
			TicketQueue<Ticket> &queue = Environment::getQueue();
			queue.add(ticket, request);
			
			ticket.wait();
		}
	}
	
	static inline void processRequests(util::array_view<Ticket::Request> requests, Ticket::Status *statuses = MPI_STATUSES_IGNORE)
	{
		assert(!requests.empty());
		
		int finished = 0;
		PMPI_Testall(requests.size(), requests.begin(), &finished, statuses);
		
		if (!finished) {
			Ticket ticket(requests, statuses);
			
			TicketQueue<Ticket> &queue = Environment::getQueue();
			queue.add(ticket, requests);
			
			ticket.wait();
		}
	}
} // namespace C


namespace Fortran {
	static inline void processRequest(Fortran::Ticket::Request &request, MPI_Fint *status = MPI_F_STATUS_IGNORE)
	{
		int finished = 0;
		int err = MPI_SUCCESS;
		pmpi_test_(&request, &finished, status, &err);
		assert(!err);
		
		if (!finished) {
			Fortran::Ticket ticket(request, status);
			
			TicketQueue<Ticket> &queue = Environment::getQueue();
			queue.add(ticket, request);
			
			ticket.wait();
		}
	}
	
	static inline void processRequests(util::array_view<Ticket::Request> requests, MPI_Fint *statuses = MPI_F_STATUSES_IGNORE)
	{
		assert(!requests.empty());
		
		int finished = 0;
		int err = MPI_SUCCESS;
		int size = requests.size();
		pmpi_testall_(&size, requests.begin(), &finished, statuses, &err);
		
		if (!finished) {
			Ticket ticket(requests, statuses);
			
			TicketQueue<Ticket> &queue = Environment::getQueue();
			queue.add(ticket, requests);
			
			ticket.wait();
		}
	}
} // namespace Fortran

#endif // PROCESS_REQUEST_H
