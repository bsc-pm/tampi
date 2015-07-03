#include <mpi.h>

#include "mpicommon.h"
#include "ticket.h"
#include "bcast.h"
#include <nanox-dev/smartpointer.hpp>

#if MPI_VERSION >=3
extern "C" {

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype,
    int root, MPI_Comm comm)
{
    int err;
    nanos::mpi::bcast( buffer, count, datatype, root, comm, &err );
    return err;
}

namespace nanos {
namespace mpi {

    typedef C::Ticket<1>::type ticket;

    shared_pointer< Ticket > &&ibcast( void *buf, int count, MPI_Datatype datatype,
            int root, MPI_Comm comm )
    {
        // TODO do not forget to assign MPI function return value to ticket error
        ticket *result = new ticket();
        MPI_Ibcast( buf, count, datatype, root, comm, &result->getData().getRequest<0>() );
        return shared_pointer<ticket>(result);
    }

}
}

#endif

