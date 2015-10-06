/*
 * Copyright (c) 2015 Barcelona Supercomputing Center. All rights
 *                    reserved.
 *
 * This file is part of OmpSs-MPI interoperability library.
 *
 * OmpSs-MPI interop. library is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <mpi.h>

#include "print.h"
#include "mpicommon.h" // Defines MPI3CONST macro

extern "C" {

void mpi_sendrecv_( MPI3CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, 
                MPI_Fint *dest, MPI_Fint *sendtag, 
                void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype,
                MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status,
                MPI_Fint *err );

void mpi_pack_size_( MPI_Fint *incount, MPI_Fint *datatype, MPI_Fint *comm, MPI_Fint *size, MPI_Fint *err );
void mpi_unpack_( MPI3CONST void *inbuf, MPI_Fint *insize, MPI_Fint *position, 
                  void *outbuf, MPI_Fint *outcount, MPI_Fint *datatype, MPI_Fint *comm, MPI_Fint *err );

void mpi_alloc_mem_( MPI_Fint *size, MPI_Fint *info, void *baseptr, MPI_Fint *err );
void mpi_free_mem_( void *base, MPI_Fint *err );


void mpi_sendrecv_replace_( void *buf, MPI_Fint *count, MPI_Fint *datatype,
                       MPI_Fint *dest, MPI_Fint *sendtag,
                       MPI_Fint *source, MPI_Fint *recvtag,
                       MPI_Fint *comm, MPI_Fint *status, MPI_Fint *err )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Sendrecv_replace" );
    /*
     * Note: this implementation tries to mimic its implementation in OpenMPI: 
     * https://github.com/open-mpi/ompi/blob/master/ompi/mpi/c/sendrecv_replace.c
     * It creates a temporary buffer where it stores the received message in
     * raw data. Then, it unpacks it in the original -- user provided -- buffer
     * and releases the reserved memory.
     */

    if (  *source == MPI_PROC_NULL
         || *dest == MPI_PROC_NULL
         || *count == 0 ) {
        return mpi_sendrecv_( buf, count, datatype, dest, sendtag,
                             buf, count, datatype, source, recvtag,
                             comm, status, err );
    } else {
        MPI_Fint size_err = MPI_SUCCESS;
        MPI_Fint alloc_err = MPI_SUCCESS;
        MPI_Fint ierror = MPI_SUCCESS;
        MPI_Fint packed_size;
        MPI_Fint info_null = MPI_Info_c2f( MPI_INFO_NULL );
        MPI_Fint recv_dt = MPI_Type_c2f( MPI_BYTE);
        void *helperbuf = NULL;

        /* setup a buffer for recv */
        mpi_pack_size_( count, datatype, comm, &packed_size, &size_err );
        mpi_alloc_mem_( &packed_size, &info_null, &helperbuf, &alloc_err );

        /* recv into temporary buffer */
        if ( size_err == MPI_SUCCESS  && alloc_err == MPI_SUCCESS ) {
            mpi_sendrecv_( buf, count, datatype, dest, sendtag,
                  helperbuf, &packed_size, &recv_dt, source, recvtag,
                  comm, status, &ierror);
        }

        /* copy into users buffer */
        if ( ierror == MPI_SUCCESS && alloc_err == MPI_SUCCESS ) {
            int position = 0;
            mpi_unpack_( helperbuf, &packed_size, &position,
               buf, count, datatype, comm, &ierror );
        }

        /* release resources */
        if ( alloc_err == MPI_SUCCESS ) {
            mpi_free_mem_( helperbuf, &ierror );
        }
        if ( err != MPI_F_STATUS_IGNORE )
            *err = ierror;
    }
}

}

