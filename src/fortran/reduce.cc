#include <mpi.h>

#include "mpicommon.h"
#include "ticket.h"
#include "reduce.h"
#include <nanox-dev/smartpointer.hpp>

#if MPI_VERSION >=3
extern "C" {
    void mpi_reduce_( const void *sendbuf, void *recvbuf, MPI_Fint *count,
                   MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root,
                   MPI_Fint *comm, MPI_Fint *err )
    {
        nanos::mpi::reduce( sendbuf, recvbuf, count, datatype, op, root, comm, err );
    }

    void mpi_ireduce_( const void *sendbuf, void *recvbuf, MPI_Fint *count,
                   MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root,
                   MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err );
}

namespace nanos {
namespace mpi {
    typedef Fortran::Ticket<1>::type ticket;

    shared_pointer< Ticket > ireduce( const void *sendbuf, void *recvbuf, MPI_Fint *count,
                MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root,
                MPI_Fint *comm )
    {
        // TODO do not forget to assign MPI function return value to ticket error
        ticket *result = new ticket();
        mpi_ireduce_( sendbuf, recvbuf, count, datatype, op, root, comm,
            &result->getData().getRequest<0>(), // Store output request into ticket
            &result->getData().getError() );    // Store output error   into ticket
        return shared_pointer<ticket>(result);
    }

}
}
#endif

