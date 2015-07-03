#ifndef BARRIER_H
#define BARRIER_H

#include "print.h"
#include "mpicommon.h"

namespace nanos {
namespace mpi {

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
