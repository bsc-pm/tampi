#ifndef BCAST_H
#define BCAST_H

// Note: no need to use an ifdef guard if MPI_VERSION < 3.
// This file should not be included in that case from the .cc file.

#include "print.h"
#include "mpicommon.h"
#include "ticket.h"
#include <nanox-dev/smartpointer.hpp>

namespace nanos {
namespace mpi {

template < typename IntType, typename DataType, typename CommType >
shared_pointer< typename TicketTraits<CommType,1>::ticket_type >
ibcast( void *buf, IntType count, DataType datatype,
            IntType root, CommType comm );

template< typename IntType, typename DataType, typename CommType, typename ErrType>
void bcast( void *buf, IntType count, DataType datatype, IntType root, 
            CommType comm, ErrType *ierror )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Bcast" );

    auto waitCond = ibcast( buf, count, datatype, root, comm );
    waitCond->wait( ierror );
}

} // namespace mpi
} // namespace nanos

#endif // BCAST_H
