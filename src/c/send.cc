#include <mpi.h>

#include "mpicommon.h"
#include "ticket.h"
#include "send.h"
#include <nanox-dev/smartpointer.hpp>

extern "C" {
    int MPI_Send( MPI3CONST void *buf, int count, MPI_Datatype datatype,
        int dest, int tag, MPI_Comm comm )
    {
        int err;
        nanos::mpi::send( buf, count, datatype, dest, tag, comm, &err );
        return err;
    }
}

namespace nanos {
namespace mpi {
    typedef C::Ticket<1>::type ticket;

    shared_pointer< ticket > isend( MPI3CONST void *buf, int count, MPI_Datatype datatype,
        int dest, int tag, MPI_Comm comm )
    {
        // TODO do not forget to assign MPI function return value to ticket error
        ticket *result = new ticket();
        MPI_Isend( buf, count, datatype, dest, tag, comm, &result->getData().getRequest<0>() );
        return shared_pointer<ticket>(result);
    }

}
}


