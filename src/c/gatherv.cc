#include <mpi.h>

#include "mpicommon.h"
#include "ticket.h"
#include "gatherv.h"
#include <nanox-dev/smartpointer.hpp>

#if MPI_VERSION >=3
extern "C" {
    int MPI_Gatherv( const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype
    recvtype,
        int root, MPI_Comm comm )
    {
        int err;
        nanos::mpi::gatherv( sendbuf, sendcount, sendtype,
                        recvbuf, recvcounts, displs, recvtype,
                        root, comm, &err );
        return err;
    }
}

namespace nanos {
namespace mpi {
    typedef C::Ticket<1>::type ticket;

    shared_pointer< Ticket> igatherv( const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype
    recvtype,
        int root, MPI_Comm comm )
    {
        // TODO do not forget to assign MPI function return value to ticket error
        ticket *result = new ticket();
        MPI_Igatherv( sendbuf, sendcount, sendtype, 
                recvbuf, recvcounts, displs, recvtype, 
                root, comm, &result->getData().getRequest<0>() );
        return shared_pointer<ticket>(result);
    }

}
}
#endif
