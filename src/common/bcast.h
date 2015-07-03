#ifndef BCAST_H
#define BCAST_H

#include "print.h"
#include "mpicommon.h"

#if MPI_VERSION >=3
namespace nanos {
namespace mpi {

template< typename IntType, typename DataType, typename CommType, 
        typename StatusType, typename ErrType>
void bcast( void *buf, IntType count, DataType datatype, IntType root, 
            CommType comm, ErrType *ierror )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Bcast" );

    auto waitCond = ibcast( buffer, count, datatype, root, comm, &request );
    waitCond->wait( ierror );
}

} // namespace mpi
} // namespace nanos
#endif // MPI_VERSION
#endif // BCAST_H
