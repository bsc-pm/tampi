
#include <mpi.h>

#include "mpicommon.h"
#include "ticket.h"
#include "recv.h"
#include <nanox-dev/smartpointer.hpp>

extern "C" {
    void mpi_recv_( void *buf, MPI_Fint *count, MPI_Fint *datatype,
        MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *err )
    {
        nanos::mpi::recv( buf, count, datatype, source, tag, comm, status, err );
    }

    void mpi_irecv_( void *buf, MPI_Fint *count, MPI_Fint *datatype,
        MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err );
}

namespace nanos {
namespace mpi {
    typedef typename TicketTraits<MPI_Fint*,1>::ticket_type ticket;

    shared_pointer< ticket > irecv( void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag,
            MPI_Fint *comm )
    {
        // TODO do not forget to assign MPI function return value to ticket error
        ticket *result = new ticket();
        mpi_irecv_( buf, count, datatype, source, tag, comm,
            &result->getData().getRequest<0>(), // Store output request into ticket
            &result->getData().getError() );    // Store output error   into ticket
        return shared_pointer<ticket>(result);
    }

}
}

