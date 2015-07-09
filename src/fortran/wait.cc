#include <mpi.h>

#include "print.h"
#include "ticket.h"
#include "mpicommon.h"
#include <nanox-dev/smartpointer.hpp>

extern "C" {

void mpi_wait_( MPI_Fint *request, MPI_Fint *status, MPI_Fint *err )
{
    using namespace nanos::mpi;
    typedef Fortran::TicketTraits<1>::ticket_type ticket;
    typedef ticket::checker_type ticket_checker;

    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Wait" );

    auto waitCond = shared_pointer<ticket>( 
                           new ticket( ticket_checker( 1, request ) )
                        );
    waitCond->wait( status, err );
}

} // extern C
