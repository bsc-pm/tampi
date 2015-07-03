#ifndef WAITALL_H
#define WAITALL_H

#include "print.h"
#include "ticket.h"
#include "mpicommon.h"
#include <nanox-dev/smartpointer.hpp>

namespace nanos {
namespace mpi {


template< typename IntType, typename ReqType, typename StatusType, typename ErrType>
void waitall( IntType count, ReqType array_of_requests[], 
            StatusType array_of_statuses[], ErrType *ierror )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Wait" );

    typedef TicketChecker<ReqType,StatusType,ErrType> ticket_checker;
    typedef Ticket< ticket_checker > ticket;

    auto waitCond = shared_pointer<ticket>( 
                           new ticket( ticket_checker( count, array_of_requests ) )
                        );
    waitCond->wait( array_of_statuses, ierror );
}

} // namespace mpi
} // namespace nanos

#endif // WAITALL_H
