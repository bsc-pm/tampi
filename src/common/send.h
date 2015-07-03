#ifndef REDUCE_H
#define REDUCE_H

#include "print.h"
#include "mpicommon.h"

namespace nanos {
namespace mpi {

template< typename IntType, typename DataType, typename CommType, typename ErrType >
void send( MPI3CONST void *buf, IntType count, DataType datatype, IntType dest,
        IntType tag, CommType comm, ErrType *ierror )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Send" );

    auto waitCond = isend( buf, count, datatype, dest, tag, comm );
    waitCond->wait( ierror );
}

} // namespace mpi
} // namespace nanos

#endif // REDUCE_H
