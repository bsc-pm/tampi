#include <mpi.h>

#include "print.h"
#include "ticket.h"
#include "mpicommon.h"
#include <nanox-dev/smartpointer.hpp>

extern "C" {

int MPI_Waitall(int count, MPI_Request array_of_requests[],
                      MPI_Status array_of_statuses[])
{
    using namespace nanos::mpi;
    typedef C::TicketTraits<>::ticket_type ticket;
    typedef ticket::checker_type ticket_checker;

    int err;
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Wait" );

    auto waitCond = shared_pointer<ticket>( 
                           new ticket( ticket_checker( count, array_of_requests ) )
                        );
     waitCond->wait( count, array_of_statuses, &err );
    return err;
}

} // extern C
