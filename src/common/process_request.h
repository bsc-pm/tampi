#ifndef PROCESS_REQUEST_H
#define PROCESS_REQUEST_H


#include <mpi.h>

#include <cassert>

#include "ticket.h"


namespace nanos {
namespace mpi {
namespace C {
	static inline void process_request(C::Ticket::Request &request)
	{
		assert(&request != nullptr);
		
		int finished = 0;
		__attribute((unused)) int err = PMPI_Test(&request, &finished, MPI_STATUS_IGNORE);
		
		if (!finished) {
			C::Ticket ticket(request);
			ticket.wait();
		}
	}
	
	
#if 0
	static inline void process_requests(C::Ticket::Request *begin, C::Ticket::Request *end)
	{
		assert(begin != nullptr);
		assert(end != nullptr);
		
		int count = std::distance(begin,end);
		int finished = 0;
		
		__attribute((unused)) int err = PMPI_Testall(count, begin, &finished, MPI_STATUSES_IGNORE);
		
		if (!finished) {
			C::Ticket ticket(begin, end);
			ticket.wait();
		}
	}
#endif
} // namespace C


namespace Fortran {
	static inline void process_request(Fortran::Ticket::Request &request)
	{
		assert(&request != nullptr);
		
		int finished = 0;
		int err = MPI_SUCCESS;
		pmpi_test_(&request, &finished, MPI_F_STATUS_IGNORE, &err);
		
		if (!finished) {
			Fortran::Ticket ticket(request);
			ticket.wait();
		}
	}
	
	
#if 0
	static inline void process_requests(Fortran::Ticket::Request *begin, Fortran::Ticket::Request *end)
	{
		assert(begin != nullptr);
		assert(end != nullptr);
		
		int count = std::distance(begin,end);
		int finished = 0;
		int err = MPI_SUCCESS;
		
		pmpi_testall_(&count, begin, &finished, MPI_F_STATUSES_IGNORE, &err);
		
		if (!finished) {
			Fortran::Ticket ticket(begin, end);
			ticket.wait();
		}
	}
#endif
} // namespace Fortran

} // namespace mpi
} // namespace nanos


#endif // PROCESS_REQUEST_H
