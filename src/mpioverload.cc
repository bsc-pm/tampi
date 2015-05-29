#include <mpi.h>
#include <nanox/nanos.h>
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
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Send" );
    int ierror = MPI_SUCCESS;

    MPI_Status status;
    MPI_Request request;

    ierror = MPI_Isend( buf, count, datatype, dest, tag, comm, &request );

    if( ierror == MPI_SUCCESS ) {
        shared_pointer<MPI_SingleTest> waitCond(new MPI_SingleTest(&request, &status) );
        nanos_polling_cond_wait( waitCond.getPointer() );

        ierror = waitCond->getResult();
    }

    return ierror;
}

int MPI_Recv( void *buf, int count, MPI_Datatype datatype,
    int source, int tag, MPI_Comm comm, MPI_Status *status )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Recv" );
    int ierror = MPI_SUCCESS;

    MPI_Status s;
    MPI_Request request;

    ierror = MPI_Irecv( buf, count, datatype, source, tag, comm, &request );

    if( ierror == MPI_SUCCESS ) {
        shared_pointer<MPI_SingleTest> waitCond(new MPI_SingleTest(&request, &s) );
        nanos_polling_cond_wait( waitCond.getPointer() );

        ierror = waitCond->getResult();
    }

    if( status != MPI_STATUS_IGNORE )
	memcpy( status, &s, sizeof(MPI_Status) );

    return ierror;
}

int MPI_Sendrecv( MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype,
                       int dest, int sendtag,
                       void *recvbuf, int recvcount, MPI_Datatype recvtype,
                       int source, int recvtag,
                       MPI_Comm comm, MPI_Status *status)
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Sendrecv" );
    int ierror = MPI_SUCCESS;
    MPI_Request reqlist[2];

    ierror = MPI_Isend( sendbuf, sendcount, sendtype, dest, sendtag, comm, &reqlist[0] );
    if( ierror == MPI_SUCCESS ) {
       ierror = MPI_Irecv( recvbuf, recvcount, recvtype, source, recvtag, comm, &reqlist[1] );
    }

    if( ierror == MPI_SUCCESS ) {
        shared_pointer<MPI_MultipleTest> waitCond;
        if( status == MPI_STATUS_IGNORE ) {
            waitCond.setPointer( new MPI_MultipleTest(2, reqlist, MPI_STATUSES_IGNORE) );
            nanos_polling_cond_wait( waitCond.getPointer() );
        } else {
            MPI_Status statuslist[2];
            waitCond.setPointer( new MPI_MultipleTest(2, reqlist, statuslist) );
            nanos_polling_cond_wait( waitCond.getPointer() );

            std::memcpy( status, &statuslist[1], sizeof(MPI_Status) );
        }
        ierror = waitCond->getResult();
    }

    return ierror;
}

int MPI_Sendrecv_replace( void *buf, int count, MPI_Datatype datatype,
                       int dest, int sendtag,
                       int source, int recvtag,
                       MPI_Comm comm, MPI_Status *status)
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Sendrecv_replace" );
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

int MPI_Wait( MPI_Request *request, MPI_Status *status )
{
    int flag = 0;
    int ierror = MPI_Test( request, &flag, status );
    if( ierror == MPI_SUCCESS && flag != 1 ) {
        shared_pointer<MPI_SingleTest> waitCond(new MPI_SingleTest(request, status) );
        nanos_polling_cond_wait( waitCond.getPointer() );

        ierror = waitCond->getResult();
    }
    return ierror;
}

int MPI_Waitall(int count, MPI_Request array_of_requests[],
                      MPI_Status array_of_statuses[])
{
    int flag = 0;
    int ierror = MPI_Testall( count, array_of_requests, &flag, array_of_statuses );
    if( ierror = MPI_SUCCESS && flag != 1 ) {
       shared_pointer<MPI_MultipleTest> waitCond;
       waitCond.setPointer( new MPI_MultipleTest(count, array_of_requests, array_of_statuses) );
       nanos_polling_cond_wait( waitCond.getPointer() );
       ierror = waitCond->getResult();
    }
    return ierror;
}

#if MPI_VERSION >=3
int MPI_Gatherv( const void *sendbuf, int sendcount, MPI_Datatype sendtype,
    void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype
recvtype,
    int root, MPI_Comm comm )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Gatherv" );
    int ierror = MPI_SUCCESS;

    MPI_Status status;
    MPI_Request request;

    ierror = MPI_Igatherv( sendbuf, sendcount, sendtype,
                           recvbuf, recvcounts, displs, recvtype,
                           root, comm, &request );

    if( ierror == MPI_SUCCESS ) {
        shared_pointer<MPI_SingleTest> waitCond = shared_pointer<MPI_SingleTest>(new MPI_SingleTest(&request, &status) );
        nanos_polling_cond_wait( waitCond.getPointer() );

        ierror = waitCond->getResult();
    }

    return ierror;
}

int MPI_Reduce( const void *sendbuf, void *recvbuf, int count,
               MPI_Datatype datatype, MPI_Op op, int root,
               MPI_Comm comm )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Reduce" );
    int ierror = MPI_SUCCESS;

    MPI_Status status;
    MPI_Request request;

    ierror = MPI_Ireduce( sendbuf, recvbuf, count,
                          datatype, op, root,
                          comm, &request );

    if( ierror == MPI_SUCCESS ) {
        shared_pointer<MPI_SingleTest> waitCond = shared_pointer<MPI_SingleTest>(new MPI_SingleTest(&request, &status) );
        nanos_polling_cond_wait( waitCond.getPointer() );

        ierror = waitCond->getResult();
    }

    return ierror;
}

int MPI_Barrier(MPI_Comm comm)
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Barrier" );
    int ierror = MPI_SUCCESS;
    MPI_Request request;
    MPI_Status status;

    ierror = MPI_Ibarrier( comm, &request );
    if( ierror == MPI_SUCCESS ) {
        shared_pointer<MPI_SingleTest> waitCond(new MPI_SingleTest(&request, &status) );
        nanos_polling_cond_wait( waitCond.getPointer() );

        // What happens if error is MPI_ERR_IN_STATUS?
        ierror = waitCond->getResult();
    }
    return ierror;
}

int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype,
    int root, MPI_Comm comm)
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Bcast" );
    int ierror = MPI_SUCCESS;
    MPI_Request request;
    MPI_Status status;

    ierror = MPI_Ibcast( buffer, count, datatype, root, comm, &request );

    if( ierror == MPI_SUCCESS ) {
        shared_pointer<MPI_SingleTest> waitCond(new MPI_SingleTest(&request, &status) );
        nanos_polling_cond_wait( waitCond.getPointer() );

        // What happens if error is MPI_ERR_IN_STATUS?
        ierror = waitCond->getResult();
    }

    return ierror;
}

#endif

}// extern C

