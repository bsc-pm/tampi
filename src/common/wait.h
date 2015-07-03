#ifndef WAIT_H
#define WAIT_H

#include "print.h"
#include "ticket.h"
#include "mpicommon.h"
#include <nanox-dev/smartpointer.hpp>

namespace nanos {
namespace mpi {

template< typename ReqType, typename StatusType, typename ErrType>
void wait( ReqType *request, StatusType *status, ErrType *ierror )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Wait" );

    typedef TicketChecker<ReqType,StatusType,ErrType,1> ticket_checker;
    typedef Ticket<ticket_checker> ticket;

    auto waitCond = shared_pointer<ticket>(
                            new ticket( ticket_checker( 1,request ) )
                        );
    waitCond->wait( status, ierror );
}

} // namespace mpi
} // namespace nanos

#endif // WAIT_H
