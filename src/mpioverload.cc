#include <mpi.h>
#include <nanox/nanos.h>
#include <nanox-dev/debug.hpp>
#include <nanox-dev/smartpointer.hpp>
#include "mpiconditionchecker.hpp"

#if MPI_VERSION >= 3
    #define MPI3CONST const
#else
    #define MPI3CONST
#endif

extern "C" {

using namespace nanos;
using namespace nanos::mpi;

int MPI_Send( MPI3CONST void *buf, int count, MPI_Datatype datatype,
    int dest, int tag, MPI_Comm comm )
{
    debug0( "Intercepted " << __FUNCTION__ );
    int ierror = MPI_SUCCESS;

    MPI_Status status;
    MPI_Request request;

    ierror = MPI_Isend( buf, count, datatype, dest, tag, comm, &request );

    if( ierror == MPI_SUCCESS ) {
        shared_pointer<MPI_SingleTest> waitCond = shared_pointer<MPI_SingleTest>(new MPI_SingleTest(&request, &status) );
        nanos_sync_cond_wait( waitCond.getPointer() );

        ierror = waitCond->getResult();
    }

    //ierror = PMPI_Send(buf, count, datatype, dest, tag, comm);
    return ierror;
}

int MPI_Recv( void *buf, int count, MPI_Datatype datatype,
    int source, int tag, MPI_Comm comm, MPI_Status *status )
{
    debug0( "Intercepted " << __FUNCTION__ );
    int ierror = MPI_SUCCESS;

    MPI_Request request;

    ierror = MPI_Irecv( buf, count, datatype, source, tag, comm, &request );

    if( ierror == MPI_SUCCESS ) {
        shared_pointer<MPI_SingleTest> waitCond = shared_pointer<MPI_SingleTest>(new MPI_SingleTest(&request, status) );
        nanos_polling_cond_wait( waitCond.getPointer() );

        ierror = waitCond->getResult();
        //delete waitCond; Note: the scheduler will take care of deleting the condition
    }

    //ierror = PMPI_Recv (buf, count, datatype, source, tag, comm, status );
    return ierror;
}

int MPI_Sendrecv( MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype,
                       int dest, int sendtag,
                       void *recvbuf, int recvcount, MPI_Datatype recvtype,
                       int source, int recvtag,
                       MPI_Comm comm, MPI_Status *status)
{
    debug0( "Intercepted " << __FUNCTION__ );
    int isend_error = MPI_SUCCESS;
    int irecv_error = MPI_SUCCESS;
    MPI_Request reqlist[2];

    isend_error = MPI_Isend( sendbuf, sendcount, sendtype, dest, sendtag, comm, &reqlist[0] );
    irecv_error = MPI_Irecv( recvbuf, recvcount, recvtype, source, recvtag, comm, &reqlist[1] );

    if( isend_error == MPI_SUCCESS && irecv_error == MPI_SUCCESS ) {
        shared_pointer<MPI_MultipleTest> waitCond;
        if( status == MPI_STATUS_IGNORE ) {
            waitCond.setPointer( new MPI_MultipleTest(2, reqlist, MPI_STATUSES_IGNORE) );
            shared_pointer<MPI_MultipleTest> waitCond = shared_pointer<MPI_MultipleTest>(new MPI_MultipleTest(2, reqlist, MPI_STATUSES_IGNORE) );
            nanos_polling_cond_wait( waitCond.getPointer() );
        } else {
            MPI_Status statuslist[2];
            waitCond.setPointer( new MPI_MultipleTest(2, reqlist, statuslist) );
            nanos_polling_cond_wait( waitCond.getPointer() );
            // Copy the value of irecv status back to the status provided by the user
            *status = statuslist[1];
        }
        irecv_error = waitCond->getResult();
    }

    return isend_error? isend_error : irecv_error;
}

int MPI_Sendrecv_replace( void *buf, int count, MPI_Datatype datatype,
                       int dest, int sendtag,
                       int source, int recvtag,
                       MPI_Comm comm, MPI_Status *status)
{
    debug0( "Intercepted " << __FUNCTION__ );
    /*
     * Note: this implementation tries to mimic its implementation in OpenMPI: 
     * https://github.com/open-mpi/ompi/blob/master/ompi/mpi/c/sendrecv_replace.c
     * It creates a temporary buffer where it stores the received message in
     * raw data. Then, it unpacks it in the original -- user provided -- buffer
     * and releases the reserved memory.
     */

    if ( source == MPI_PROC_NULL || dest == MPI_PROC_NULL || count == 0 ) {
        return MPI_Sendrecv( buf, count, datatype, dest, sendtag,
                             buf, count, datatype, source, recvtag,
                             comm, status );
    } else {
        int ierror = MPI_SUCCESS;
        int alloc = MPI_SUCCESS;
        MPI_Request reqlist[2];
        int packed_size;
        void *helperbuf = NULL;

        /* setup a buffer for recv */
        ierror = MPI_Pack_size( count, datatype, comm, &packed_size );
        alloc = MPI_Alloc_mem( packed_size, MPI_INFO_NULL, &helperbuf );

        /* recv into temporary buffer */
        if ( ierror == MPI_SUCCESS  && alloc == MPI_SUCCESS ) {
            ierror = MPI_Sendrecv( buf, count, datatype, dest, sendtag,
                           helperbuf, packed_size, MPI_BYTE, source, recvtag,
                           comm, status);
        }

        /* copy into users buffer */
        if ( ierror == MPI_SUCCESS && alloc == MPI_SUCCESS ) {
            int position = 0;
            ierror = MPI_Unpack( helperbuf, packed_size, &position,
                        buf, count, datatype, comm );
        }

        /* release resources */
        if ( alloc == MPI_SUCCESS ) {
            ierror = MPI_Free_mem( helperbuf );
        }
        return ierror;
    }
}

#if MPI_VERSION >=3
int MPI_Barrier(MPI_Comm comm)
{
    debug0( "Intercepted " << __FUNCTION__ );
    int ierror = MPI_SUCCESS;
    MPI_Request request;
    MPI_Status status;

    ierror = MPI_Ibarrier( comm, &request );

    if( ierror == MPI_SUCCESS ) {
        shared_pointer<MPI_SingleTest> waitCond = shared_pointer<MPI_SingleTest>(new MPI_SingleTest(&request, &status) );
        nanos_polling_cond_wait( waitCond.getPointer() );

        // What happens if error is MPI_ERR_IN_STATUS?
        ierror = waitCond->getResult();
    }

    return ierror;
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype,
    int root, MPI_Comm comm)
{
    debug0( "Intercepted " << __FUNCTION__ );
    int ierror = MPI_SUCCESS;
    MPI_Request request;
    MPI_Status status;

    ierror = MPI_Ibcast( buffer, count, datatype, root, comm, &request );

    if( ierror == MPI_SUCCESS ) {
        shared_pointer<MPI_SingleTest> waitCond = shared_pointer<MPI_SingleTest>(new MPI_SingleTest(&request, &status) );
        nanos_polling_cond_wait( waitCond.getPointer() );

        // What happens if error is MPI_ERR_IN_STATUS?
        ierror = waitCond->getResult();
    }

    return ierror;
}
#endif

}// extern C

