#ifndef BARRIER_H
#define BARRIER_H

#include "print.h"
#include "mpicommon.h"
#include "ticket.h"
#include <nanox-dev/smartpointer.hpp>

namespace nanos {
namespace mpi {

template< typename CommType >
shared_pointer< typename TicketTraits<CommType,1>::ticket_type >
ibarrier( CommType comm );

template< typename CommType, typename ErrType >
void barrier( CommType comm, ErrType *ierror )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Barrier" );

    auto waitCond = ibarrier( comm );
    waitCond->wait( ierror );
}

} // namespace mpi
} // namespace nanos

#endif // BARRIER_H
