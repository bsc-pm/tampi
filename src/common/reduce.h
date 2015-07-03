#ifndef REDUCE_H
#define REDUCE_H

#include "print.h"
#include "mpicommon.h"

#if MPI_VERSION >=3
namespace nanos {
namespace mpi {

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

#endif // MPI_VERSION

#endif // REDUCE_H
