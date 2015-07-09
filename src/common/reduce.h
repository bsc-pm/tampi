#ifndef REDUCE_H
#define REDUCE_H

#include "print.h"
#include "mpicommon.h"
#include "ticket.h"
#include <nanox-dev/smartpointer.hpp>

namespace nanos {
namespace mpi {

template< typename IntType, typename DataType, 
	  typename OpType, typename CommType >
shared_pointer< typename TicketTraits<CommType,1>::ticket_type >
ireduce( const void *sendbuf, void *recvbuf, IntType count,
            DataType datatype, OpType op, IntType root,
            CommType comm );

template< typename IntType, typename DataType, typename OpType, 
        typename CommType, typename ErrType >
void reduce( const void *sendbuf, void *recvbuf, IntType count,
            DataType datatype, OpType op, IntType root, 
            CommType comm, ErrType *ierror )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Reduce" );

    auto waitCond = 
            ireduce( sendbuf, recvbuf, count, datatype, op, root, comm );
    waitCond->wait( ierror );
}

} // namespace mpi
} // namespace nanos

#endif // REDUCE_H
