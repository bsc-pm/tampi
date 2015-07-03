#include <mpi.h>

#include "mpicommon.h"
#include "wait.h"

extern "C" {

int MPI_Wait( MPI_Request *request, MPI_Status *status )
{
    int err;
    nanos::mpi::wait( request, status, &err );
    return err;
}

}
