#include <mpi.h> // defines MPI_VERSION

#if MPI_VERSION >=3

#include "mpicommon.h"
#include "ticket.h"
#include "barrier.h"
#include <nanox-dev/smartpointer.hpp>

extern "C" {
    int MPI_Barrier( MPI_Comm comm )
    {
        int err;
        nanos::mpi::barrier( comm, &err );
        return err;
    }
} // extern C

namespace nanos {
namespace mpi {
    typedef typename TicketTraits<MPI_Comm,1>::ticket_type ticket;

    template<>
    shared_pointer< ticket > ibarrier( MPI_Comm comm )
    {
        ticket *result = new ticket();
        int err = MPI_Ibarrier( comm, &result->getData().getRequest<0>() );
        result->getData().setError( err );

        return shared_pointer<ticket>(result);
    }

} // namespace mpi
} // namespace nanos

#endif // MPI_VERSION

