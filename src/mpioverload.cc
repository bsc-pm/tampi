#include <mpi.h>
#include <nanox/nanos.h>
#include "mpiconditionchecker.hpp"

#if MPI_VERSION >= 3
    #define MPI3CONST const
#else
    #define MPI3CONST
#endif

extern "C" {

int MPI_Send( MPI3CONST void *buf, int count, MPI_Datatype datatype,
    int dest, int tag, MPI_Comm comm )
{
    int ierror = MPI_SUCCESS;

    MPI_Status status;
    MPI_Request request;

    ierror = MPI_Isend( buf, count, datatype, dest, tag, comm, &request );

    nanos::mpi::SingleTestCond waitCond(&request, &status);
    nanos_sync_cond_wait( &waitCond );

    //ierror = PMPI_Send(buf, count, datatype, dest, tag, comm);
    return ierror + status.MPI_ERROR;
}

int MPI_Recv( void *buf, int count, MPI_Datatype datatype,
    int source, int tag, MPI_Comm comm, MPI_Status *status )
{
    int ierror = MPI_SUCCESS;

    MPI_Request request;

    ierror = MPI_Irecv( buf, count, datatype, source, tag, comm, &request );
    std::cout << "Waiting for receive" << std::endl;

    nanos::mpi::SingleTestCond waitCond(&request, status);
    nanos_polling_cond_wait( &waitCond );

    //ierror = PMPI_Recv (buf, count, datatype, source, tag, comm, status );
    return ierror + status->MPI_ERROR;
}

#if MPI_VERSION >=3

int MPI_Barrier(MPI_Comm comm)
{
    int ierror = MPI_SUCCESS;
    MPI_Request request;
    MPI_Status status;

    ierror = MPI_Ibarrier( comm, &request );

    nanos::mpi::SingleTestCond waitCond(&request, &status);
    nanos_polling_cond_wait( &waitCond );

    return ierror + status.MPI_ERROR;
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype,
    int root, MPI_Comm comm)
{
    int ierror = MPI_SUCCESS;
    MPI_Request request;
    MPI_Status status;

    ierror = MPI_Ibcast( buffer, count, datatype, root, comm, &request );

    nanos::mpi::SingleTestCond waitCond( &request, &status );
    nanos_polling_cond_wait( &waitCond );

    return ierror + status.MPI_ERROR;
}

int MPI_Sendrecv( MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype,
                       int dest, int sendtag,
                       void *recvbuf, int recvcount, MPI_Datatype recvtype,
                       int source, int recvtag,
                       MPI_Comm comm, MPI_Status *status)
{
    int ierror = MPI_SUCCESS;
    MPI_Request sendreq;
    MPI_Request recvreq;
    MPI_Status sendstat;

    ierror = MPI_Isend( sendbuf, sendcount, sendtype, dest, sendtag, comm, &sendreq );
    ierror = MPI_Irecv( recvbuf, recvcount, recvtype, source, recvtag, comm, &recvreq );

    nanos::mpi::MultipleTestCond waitCond({sendreq, recvreq}, {sendstat,*status});
    nanos_polling_cond_wait( &waitCond );

    return ierror + status->MPI_ERROR;
}

#endif

}// extern C

