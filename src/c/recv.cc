#include <mpi.h>

#include "mpicommon.h"
#include "ticket.h"
#include "recv.h"
#include <nanox-dev/smartpointer.hpp>

extern "C" {
    int MPI_Recv( void *buf, int count, MPI_Datatype datatype,
        int source, int tag, MPI_Comm comm, MPI_Status *status )
    {
        int err;
        nanos::mpi::recv( buf, count, datatype, source, tag, comm, status, &err );
        return err;
    }
} // extern C

namespace nanos {
namespace mpi {
    typedef typename TicketTraits<MPI_Comm,1>::ticket_type ticket;

    shared_pointer<ticket> irecv( void *buf, int count, MPI_Datatype datatype, int source, int tag,
            MPI_Comm comm )
    {
        // TODO do not forget to assign MPI function return value to ticket error
        ticket *result = new ticket();
        MPI_Irecv( buf, count, datatype, source, tag, comm, &result->getData().getRequest<0>() );
        return shared_pointer<ticket>(result);
    }
    
} // namespace mpi
} // namespace nanos
