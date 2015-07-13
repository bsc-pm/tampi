#include <mpi.h> // defines MPI_VERSION

#if MPI_VERSION >=3

#include "mpicommon.h"
#include "ticket.h"
#include "gatherv.h"
#include <nanox-dev/smartpointer.hpp>

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
} // extern C

namespace nanos {
namespace mpi {
    typedef typename TicketTraits<MPI_Comm,1>::ticket_type ticket;

    template<>
    shared_pointer<ticket> igatherv( const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, const int recvcounts[], const int displs[], 
	     MPI_Datatype recvtype, int root, MPI_Comm comm )
    {
        // TODO do not forget to assign MPI function return value to ticket error
        ticket *result = new ticket();
        int err = MPI_Igatherv( sendbuf, sendcount, sendtype, 
                    recvbuf, recvcounts, displs, recvtype, 
                    root, comm, &result->getData().getRequest<0>() );
        result->getData().setError( err );

        return shared_pointer<ticket>(result);
    }

} // namespace mpi
} // namespace nanos

#endif // MPI_VERSION

