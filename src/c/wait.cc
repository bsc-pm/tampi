#include <mpi.h>

#include "print.h"
#include "ticket.h"
#include "mpicommon.h"
#include <nanox-dev/smartpointer.hpp>

extern "C" {

int MPI_Wait( MPI_Request *request, MPI_Status *status )
{
    using namespace nanos::mpi;
    typedef C::TicketTraits<1>::ticket_type ticket;
    typedef ticket::checker_type ticket_checker;

    int err;
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Wait" );

    auto waitCond = shared_pointer<ticket>( 
                           new ticket( ticket_checker( 1, request ) )
                        );

    waitCond->wait( status, &err );
    return err;
}

} // extern C

