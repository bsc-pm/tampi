#include <mpi.h>

#include "mpicommon.h"
#include "ticket.h"
#include "gatherv.h"
#include <nanox-dev/smartpointer.hpp>

#if MPI_VERSION >=3
extern "C" {
    void mpi_gatherv_( const void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype,
        void *recvbuf, MPI_Fint recvcounts[], MPI_Fint displs[], MPI_Fint *recvtype,
        MPI_Fint *root, MPI_Fint *comm, MPI_Fint *err )
    {
        nanos::mpi::gatherv( sendbuf, sendcount, sendtype,
                        recvbuf, recvcounts, displs, recvtype,
                        root, comm, err );
    }

    void mpi_igatherv_( const void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype,
        void *recvbuf, MPI_Fint recvcounts[], MPI_Fint displs[], MPI_Fint *recvtype,
        MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err );
}

namespace nanos {
namespace mpi {
    typedef Fortran::Ticket<1>::type ticket;

    template<>
    shared_pointer< Ticket> igatherv( const void *sendbuf, MPI_Fint *sendcount, MPI_MPI_Fint *sendtype,
        void *recvbuf, MP_Fint recvcounts[], MPI_Fint displs[], MPI_Fint *recvtype,
        MPI_Fint *root, MPI_Fint *comm )
    {
        // TODO do not forget to assign MPI function return value to ticket error
        ticket *result = new ticket();
        mpi_igatherv_( sendbuf, sendcount, sendtype, 
                recvbuf, recvcounts, displs, recvtype, 
                root, comm, 
                &result->getData().getRequest<0>(), // Store output request into ticket
                &result->getData()->getError() );   // Store output error   into ticket
        return shared_pointer<ticket>(result);
    }

}
}
#endif

