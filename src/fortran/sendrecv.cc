#include <mpi.h>

#include "mpicommon.h"
#include "ticket.h"
#include "sendrecv.h"
#include <nanox-dev/smartpointer.hpp>

extern "C" {

void mpi_sendrecv_( MPI3CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype,
                       MPI_Fint *dest, MPI_Fint *sendtag,
                       void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype,
                       MPI_Fint *source, MPI_Fint *recvtag,
                       MPI_Fint *comm, MPI_Fint *status, MPI_Fint *err )
{
    nanos::mpi::sendrecv( sendbuf, sendcount, sendtype, dest, sendtag,
                        recvbuf, recvcount, recvtype, source, recvtag,
                        comm, status, err );
}

void mpi_isend_( MPI3CONST void *sendbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest,
            MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err );

void mpi_irecv_( void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *src,
            MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err );

}

namespace nanos {
namespace mpi {
    typedef typename TicketTraits<MPI_Fint*,1>::ticket_type ticket;

    shared_pointer<ticket> isendrecv( MPI3CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype,
                    MPI_Fint *dest, MPI_Fint *sendtag,
                    void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype,
                    MPI_Fint *source, MPI_Fint *recvtag,
                    MPI_Fint *comm )
    {
        ticket *result = new ticket();
        mpi_isend_( sendbuf, sendcount, sendtype, dest, sendtag, comm,
                &result->getData().getRequest<0>(), // Store output request into ticket
                &result->getData().getError() );    // Store output error   into ticket
        mpi_irecv_( recvbuf, recvcount, recvtype, source, recvtag, comm,
                &result->getData().getRequest<1>(), // Store output request into ticket
                &result->getData().getError() );    // Store output error   into ticket
        return shared_pointer<ticket>(result);
    }
}
}
