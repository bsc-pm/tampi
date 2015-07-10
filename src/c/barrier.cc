#include <mpi.h>

#include "mpicommon.h"
#include "ticket.h"
#include "barrier.h"
#include <nanox-dev/smartpointer.hpp>

#if MPI_VERSION >=3

extern "C" {
    int MPI_Barrier( MPI_Comm comm )
    {
        int err;
        nanos::mpi::barrier( comm, &err );
        return err;
    }
}

namespace nanos {
namespace mpi {
    typedef typename TicketTraits<MPI_Comm,1>::ticket_type ticket;

    template<>
    shared_pointer< ticket_t > ibarrier( MPI_Comm comm )
    {
        // TODO do not forget to assign MPI function return value to ticket error
        ticket *result = new ticket();
        MPI_Ibarrier( comm, &result->getData().getRequest<0>() );
        return shared_pointer<ticket>(result);
    }

}
}

#endif
