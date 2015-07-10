#ifndef RECV_H
#define RECV_H

#include "print.h"
#include "mpicommon.h"
#include "ticket.h"
#include <nanox-dev/smartpointer.hpp>

namespace nanos {
namespace mpi {

template< typename IntType, typename DataType, typename CommType >
shared_pointer< typename TicketTraits<CommType,1>::ticket_type >
irecv( void *buf, IntType count, DataType datatype, IntType source, IntType tag,
            CommType comm );

template< typename IntType, typename DataType, 
        typename CommType, typename StatusType, typename ErrType >
void recv( void *buf, IntType count, DataType datatype, 
        IntType source, IntType tag, CommType comm, StatusType *status,
        ErrType *ierror )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Recv" );

    auto waitCond = irecv( buf, count, datatype, source, tag, comm );
    waitCond->wait( status, ierror );
}

} // namespace mpi
} // namespace nanos

#endif // RECV_H