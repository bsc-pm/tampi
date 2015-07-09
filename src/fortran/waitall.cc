#include <mpi.h>

#include "print.h"
#include "ticket.h"
#include "mpicommon.h"
#include <nanox-dev/smartpointer.hpp>

extern "C" {

void mpi_waitall_( MPI_Fint *count, MPI_Fint array_of_requests[],
                      MPI_Fint array_of_statuses[], MPI_Fint *err )
{
    using namespace nanos::mpi;
    typedef Fortran::TicketTraits<0>::ticket_type ticket;
    typedef ticket::checker_type ticket_checker;

    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Waitall" );

    auto waitCond = shared_pointer<ticket>( 
                           new ticket( ticket_checker( *count, array_of_requests ) )
                        );
    waitCond->wait( *count, array_of_statuses, err );
}

} // extern C
