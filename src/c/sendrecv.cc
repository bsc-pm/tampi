#include <mpi.h>

#include "mpicommon.h"
#include "ticket.h"
#include "sendrecv.h"
#include <nanox-dev/smartpointer.hpp>

extern "C" {

int MPI_Sendrecv( MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype,
                       int dest, int sendtag,
                       void *recvbuf, int recvcount, MPI_Datatype recvtype,
                       int source, int recvtag,
                       MPI_Comm comm, MPI_Status *status)
{
    int err;
    nanos::mpi::sendrecv( sendbuf, sendcount, sendtype, dest, sendtag,
                        recvbuf, recvcount, recvtype, source, recvtag,
                        comm, status, &err );
    return err;
}

}

namespace nanos {
namespace mpi {
    typedef C::Ticket<2>::type ticket;

    shared_pointer<ticket> isendrecv( MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype,
                    int dest, int sendtag,
                    void *recvbuf, int recvcount, MPI_Datatype recvtype,
                    int source, int recvtag,
                    MPI_Comm comm )
    {
        // TODO do not forget to assign MPI function return value to ticket error
        ticket *result = new ticket();
        MPI_Isend( sendbuf, sendcount, sendtype, dest, sendtag,
                    comm, &result->getData().getRequest<0>() );
        MPI_Irecv( recvbuf, recvcount, recvtype, source, recvtag,
                    comm, &result->getData().getRequest<1>() );
        return shared_pointer<ticket>(result);
    }
}
}
