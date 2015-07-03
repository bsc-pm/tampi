#include <mpi.h>

#include "mpicommon.h"
#include "waitall.h"

extern "C" {

int MPI_Waitall(int count, MPI_Request array_of_requests[],
                      MPI_Status array_of_statuses[])
{
    int err;
    nanos::mpi::waitall( count, array_of_requests, array_of_statuses, &err );
    return err;
}

}
