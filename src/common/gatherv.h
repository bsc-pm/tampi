#ifndef BARRIER_H
#define BARRIER_H

#include "print.h"
#include "mpicommon.h"
#include "ticket.h"
#include <nanox-dev/smartpointer.hpp>

namespace nanos {
namespace mpi {

template< typename IntType, typename IntArrayType, typename DataType, typename CommType >
shared_pointer< typename TicketTraits<CommType,1>::ticket_type >
igatherv( const void *sendbuf, IntType sendcount, DataType sendtype,
        void *recvbuf, IntArrayType recvcounts, IntArrayType displs,
	DataType recvtype, IntType root, CommType comm );

template< typename IntType, typename IntArrayType, typename DataType, typename CommType, typename ErrType >
void gatherv( const void *sendbuf, IntType sendcount, DataType sendtype,
            void *recvbuf, IntArrayType recvcounts, 
            IntArrayType displs, DataType recvtype,
            IntType root, CommType comm, ErrType *ierror )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Gatherv" );

    auto waitCond =
            igatherv( sendbuf, sendcount, sendtype,
                    recvbuf, recvcounts, displs, recvtype,
                    root, comm );
    waitCond->wait( ierror );
}

} // namespace mpi
} // namespace nanos

#endif // BARRIER_H

