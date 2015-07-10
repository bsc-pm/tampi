#ifndef SENDRECV_H
#define SENDRECV_H

#include "print.h"
#include "mpicommon.h"
#include "ticket.h"
#include <nanox-dev/smartpointer.hpp>

namespace nanos {
namespace mpi {

template< typename IntType, typename DataType, typename CommType >
shared_pointer< typename TicketTraits<CommType,1>::ticket_type >
isendrecv( MPI3CONST void *sendbuf, IntType sendcount, DataType sendtype,
            IntType dest, IntType sendtag,
            void *recvbuf, IntType recvcount, DataType recvtype,
            IntType source, IntType recvtag, CommType comm );

template< typename IntType, typename DataType, typename CommType, 
        typename StatusType, typename ErrType>
void sendrecv( MPI3CONST void *sendbuf, IntType sendcount, DataType sendtype,
            IntType dest, IntType sendtag,
            void *recvbuf, IntType recvcount, DataType recvtype,
            IntType source, IntType recvtag,
            CommType comm, StatusType *status, ErrType *ierror )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Sendrecv" );

    auto waitCond =
        isendrecv( sendbuf, sendcount, sendtype, dest, sendtag, 
                    recvbuf, recvcount, recvtype, source, recvtag, comm );
    waitCond->wait( status, ierror );
}

} // namespace mpi
} // namespace nanos

#endif // SENDRECV_H