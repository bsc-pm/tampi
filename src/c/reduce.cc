#include <mpi.h>

#include "mpicommon.h"
#include "ticket.h"
#include "reduce.h"
#include <nanox-dev/smartpointer.hpp>

#if MPI_VERSION >=3
extern "C" {
    int MPI_Reduce( const void *sendbuf, void *recvbuf, int count,
                   MPI_Datatype datatype, MPI_Op op, int root,
                   MPI_Comm comm )
    {
        int err;
        nanos::mpi::reduce( sendbuf, recvbuf, count, datatype, op, root, comm, &err );
        return err;
    }
}

namespace nanos {
namespace mpi {
    typedef C::Ticket<1>::type ticket;

    shared_pointer< Ticket > &&ireduce( const void *sendbuf, void *recvbuf, int count,
                MPI_Datatype datatype, MPI_Op op, int root,
                MPI_Comm comm )
    {
        // TODO do not forget to assign MPI function return value to ticket error
        ticket *result = new ticket();
        MPI_Ireduce( sendbuf, recvbuf, count, datatype, op, root, comm, &result->getData().getRequest<0>() );
        return shared_pointer<ticket>(result);
    }

}
}
#endif
