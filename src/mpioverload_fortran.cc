
#include <mpi.h>

#define SIZEOF_MPI_STATUS sizeof(MPI_Status)/sizeof(int)

#if MPI_VERSION >= 3
    #define MPI3CONST const
#else
    #define MPI3CONST
#endif

extern "C" {

void mpi_send_( MPI3CONST void *buf, MPI_Fint *count, MPI_Fint *datatype,
        MPI_Fint *dest, MPI_Fint *tag, 
        MPI_Fint *comm, MPI_Fint *ierror )
{
    MPI_Datatype dt = MPI_Type_f2c( *datatype );
    MPI_Comm c = MPI_Comm_f2c( *comm );
    *ierror = MPI_Send( buf, *count, dt, *dest, *tag, c );
}

void mpi_recv_( void *buf, MPI_Fint *count, MPI_Fint *datatype,
        MPI_Fint *source, MPI_Fint *tag,
        MPI_Fint *comm, MPI_Fint *status,
        MPI_Fint *ierror )
{
    MPI_Datatype dt = MPI_Type_f2c( *datatype );
    MPI_Comm c = MPI_Comm_f2c( *comm );

    if( status != MPI_F_STATUS_IGNORE ) {
        MPI_Status s;
        *ierror = MPI_Recv( buf, *count, dt, *source, *tag, c, &s );
        MPI_Status_c2f( &s, status );
    } else {
        *ierror = MPI_Recv( buf, *count, dt, *source, *tag, c, MPI_STATUS_IGNORE );
    }
}

void mpi_sendrecv_( MPI3CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype,
        MPI_Fint *dest, MPI_Fint *sendtag,
        void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype,
        MPI_Fint *source, MPI_Fint *recvtag,
        MPI_Fint *comm, MPI_Fint *status,
        MPI_Fint *ierror )
{
    MPI_Datatype sdt = MPI_Type_f2c( *sendtype );
    MPI_Datatype rdt = MPI_Type_f2c( *recvtype );
    MPI_Comm c = MPI_Comm_f2c( *comm );

    if( status != MPI_F_STATUS_IGNORE ) {
        MPI_Status s;
        *ierror = MPI_Sendrecv( sendbuf, *sendcount, sdt, *dest, *sendtag,
                            recvbuf, *recvcount, rdt, *source, *recvtag,
                            c, &s );
        MPI_Status_c2f( &s, status );
    } else {
        *ierror = MPI_Sendrecv( sendbuf, *sendcount, sdt, *dest, *sendtag,
                            recvbuf, *recvcount, rdt, *source, *recvtag,
                            c, MPI_STATUS_IGNORE );
    }
}

void mpi_sendrecv_replace_( void *buf, MPI_Fint *count, MPI_Fint *datatype,
        MPI_Fint *dest, MPI_Fint *sendtag,
        MPI_Fint *source, MPI_Fint *recvtag,
        MPI_Fint *comm, MPI_Fint *status,
        MPI_Fint *ierror )
{
    MPI_Datatype dt = MPI_Type_f2c( *datatype );
    MPI_Comm c = MPI_Comm_f2c( *comm );

    if( status != MPI_F_STATUS_IGNORE ) {
        MPI_Status s;
        *ierror = MPI_Sendrecv_replace( buf, *count, dt, 
                            *dest, *sendtag,
                            *source, *recvtag,
                            c, &s );
        MPI_Status_c2f( &s, status );
    } else {
        *ierror = MPI_Sendrecv_replace( buf, *count, dt, 
                            *dest, *sendtag,
                            *source, *recvtag,
                            c, MPI_STATUS_IGNORE );
    }
}

void mpi_wait_( MPI_Fint *request, MPI_Fint *status, MPI_Fint *ierror )
{
    MPI_Request req = MPI_Request_f2c( *request );
    if( status != MPI_F_STATUS_IGNORE ) {
        MPI_Status st;
        MPI_Status_f2c( status, &st );
        *ierror = MPI_Wait( &req, &st );
        MPI_Status_c2f( &st, status );
    } else {
        *ierror = MPI_Wait( &req, MPI_STATUS_IGNORE );
    }
}

void mpi_waitall_( MPI_Fint * count, MPI_Fint array_of_requests[],
        MPI_Fint array_of_statuses[][SIZEOF_MPI_STATUS], MPI_Fint * ierror )
{
    MPI_Request requests[*count];
    for( int r = 0; r < *count; r++ ) {
        requests[r] = MPI_Request_f2c( array_of_requests[r] );
    }

    if( ((MPI_Fint*)array_of_statuses) != MPI_F_STATUSES_IGNORE ) {
        MPI_Status  statuses[*count];
        *ierror = MPI_Waitall( *count, requests, statuses );

        for( int r = 0; r < *count; r++ ) {
             MPI_Status_c2f( &statuses[r], array_of_statuses[r] );
        }
    } else {
        *ierror = MPI_Waitall( *count, requests, MPI_STATUSES_IGNORE );
    }
}

#if MPI_VERSION >=3
void mpi_barrier_( MPI_Fint *comm, MPI_Fint *ierror )
{
    MPI_Comm c  = MPI_Comm_f2c( *comm );
    *ierror = MPI_Barrier( c );
}

void mpi_bcast_( void *buffer, MPI_Fint *count, MPI_Fint *datatype,
        MPI_Fint *root, MPI_Fint *comm,
        MPI_Fint *ierror )
{
    MPI_Datatype dt = MPI_Type_f2c( *datatype );
    MPI_Comm c = MPI_Comm_f2c( *comm );

    *ierror = MPI_Bcast( buffer, *count, dt, 
                            *root, c );
}

void mpi_gatherv_(MPI3CONST void *sendbuf, MPI_Fint *sendcnt, MPI_Fint *sendtype,
                            void *recvbuf, MPI3CONST MPI_Fint *recvcnts, MPI3CONST MPI_Fint *displs, MPI_Fint *recvtype,
                            MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror )
{
    MPI_Datatype send_dt = MPI_Type_f2c( *sendtype );
    MPI_Datatype recv_dt = MPI_Type_f2c( *recvtype );
    MPI_Comm c = MPI_Comm_f2c( *comm );

    *ierror = MPI_Gatherv( sendbuf, *sendcnt, send_dt, recvbuf, recvcnts, displs, recv_dt, *root, c );
}

void mpi_reduce_( MPI3CONST void *sendbuf, void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, 
                  MPI_Fint *op, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *ierror )
{
    MPI_Datatype dt = MPI_Type_f2c( *datatype );
    MPI_Op c_op = MPI_Op_f2c( *op );
    MPI_Comm c = MPI_Comm_f2c( *comm );

    *ierror = MPI_Reduce( sendbuf, recvbuf, *count, dt, c_op, *root, c );
}
#endif

}
