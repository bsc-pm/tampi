#include <mpi.h>

#if MPI_VERSION >=3

#include "mpicommon.h"
#include "ticket.h"
#include "reduce.h"
#include <nanox-dev/smartpointer.hpp>

extern "C" {
    int MPI_Reduce( const void *sendbuf, void *recvbuf, int count,
                   MPI_Datatype datatype, MPI_Op op, int root,
                   MPI_Comm comm )
    {
        int err;
        nanos::mpi::reduce( sendbuf, recvbuf, count, datatype, op, root, comm, &err );
        return err;
    }
} // extern C

namespace nanos {
namespace mpi {
    typedef typename TicketTraits<MPI_Comm,1>::ticket_type ticket;

    template<>
    shared_pointer< ticket > ireduce( const void *sendbuf, void *recvbuf, int count,
                MPI_Datatype datatype, MPI_Op op, int root,
                MPI_Comm comm )
    {
        // TODO do not forget to assign MPI function return value to ticket error
        ticket *result = new ticket();
        MPI_Ireduce( sendbuf, recvbuf, count, datatype, op, root, comm, &result->getData().getRequest<0>() );
        return shared_pointer<ticket>(result);
    }

} // namespace mpi
} // namespace nanos

#endif // MPI_VERSION
