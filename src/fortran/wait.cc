#include <mpi.h>

#include "print.h"
#include "ticket.h"
#include "mpicommon.h"
#include <nanox-dev/smartpointer.hpp>

extern "C" {

void mpi_wait_( MPI_Fint *request, MPI_Fint *status, MPI_Fint *err )
{
    using namespace nanos::mpi;
    using namespace nanos::mpi::Fortran;
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Wait" );

    typedef TicketChecker<request_type,status_type,int_type,1> ticket_checker;
    typedef nanos::mpi::Ticket< ticket_checker > ticket;

    auto waitCond = shared_pointer<ticket>( 
                           new ticket( ticket_checker( 1, request ) )
                        );
    waitCond->wait( status, err );
}

}

