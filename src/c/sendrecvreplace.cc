#include <mpi.h>

#include "print.h"

extern "C" {

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

}
