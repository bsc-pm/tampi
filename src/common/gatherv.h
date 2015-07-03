#ifndef BARRIER_H
#define BARRIER_H

#include "print.h"
#include "mpicommon.h"

#if MPI_VERSION >=3
namespace nanos {
namespace mpi {

template< typename IntType, typename DataType, typename CommType, typename ErrType >
void gatherv( const void *sendbuf, IntType sendcount, DataType sendtype,
            void *recvbuf, const IntType recvcounts[], 
            const IntType displs[], DataType recvtype,
            IntType root, CommType comm, ErrType *ierror )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Gatherv" );

    auto wait_cond =
            igatherv( sendbuf, sendcount, sendtype,
                    recvbuf, recvcounts, displs, recvtype,
                    root, comm );
    waitCond->wait( ierror );
}

} // namespace mpi
} // namespace nanos

#endif // MPI_VERSION

#endif // BARRIER_H
