#include <mpi.h>

#include "mpicommon.h"
#include "ticket.h"
#include "bcast.h"
#include <nanox-dev/smartpointer.hpp>

#if MPI_VERSION >=3
extern "C" {

void mpi_bcast_(void *buffer, MPI_Fint *count, MPI_Fint *datatype,
    MPI_Fint *root, MPI_Fint *comm, MPI_Fint *err )
{
    nanos::mpi::bcast( buffer, count, datatype, root, comm, err );
}

void mpi_ibcast_(void *buffer, MPI_Fint *count, MPI_Fint *datatype,
    MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err );

}

namespace nanos {
namespace mpi {

    typedef Fortran::Ticket<1>::type ticket;

    shared_pointer< Ticket > ibcast( void *buf, MPI_Fint *count, MPI_Fint *datatype,
            MPI_Fint *root, MPI_Fint *comm )
    {
        ticket *result = new ticket();
        mpi_ibcast_( buf, count, datatype, root, comm,
            &result->getData().getRequest<0>(),  // Store output request into ticket
            &result->getError() );               // Store output error   into ticket
        return shared_pointer<ticket>(result);
    }

}
}

#endif


