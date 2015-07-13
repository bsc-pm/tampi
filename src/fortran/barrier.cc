#include <mpi.h>

#include "mpicommon.h"
#include "ticket.h"
#include "barrier.h"
#include <nanox-dev/smartpointer.hpp>

#if MPI_VERSION >=3

extern "C" {
    void mpi_barrier_( MPI_Fint *comm, MPI_Fint *err )
    {
        nanos::mpi::barrier( comm, err );
    }

    void mpi_ibarrier_( MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err );
}

namespace nanos {
namespace mpi {
    typedef typename TicketTraits<MPI_Fint*,1>::ticket_type ticket;

    template<>
    shared_pointer< ticket > ibarrier( MPI_Fint *comm )
    {
        ticket *result = new ticket();
        mpi_ibarrier_( comm, 
                &result->getData().getRequest<0>(),  // Store output request into ticket
                &result->getData().getError() );               // Store output error   into ticket
        return shared_pointer<ticket>(result);
    }

}
}

#endif

