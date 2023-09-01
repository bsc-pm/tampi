/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "TAMPI_Decl.h"

#include "Declarations.hpp"
#include "Environment.hpp"
#include "Interface.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {

void mpi_alloc_mem_(MPI_Fint *size, MPI_Fint *info, void *baseptr, MPI_Fint *err);
void mpi_free_mem_(void *base, MPI_Fint *err);

void mpi_sendrecv_(
		void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *dest, MPI_Fint *sendtag,
		void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *source, MPI_Fint *recvtag,
		MPI_Fint *comm, MPI_Fint *status, MPI_Fint *err);

void mpi_pack_size_(MPI_Fint *incount, MPI_Fint *datatype, MPI_Fint *comm, MPI_Fint *size, MPI_Fint *err);
void mpi_unpack_(void *inbuf, MPI_Fint *insize, MPI_Fint *position, void *outbuf, MPI_Fint *outcount, MPI_Fint *datatype, MPI_Fint *comm, MPI_Fint *err);


void mpi_sendrecv_replace_(
		void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *sendtag,
		MPI_Fint *source, MPI_Fint *recvtag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *err)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		if (*source == MPI_PROC_NULL || *dest == MPI_PROC_NULL || *count == 0) {
			return mpi_sendrecv_(
					buf, count, datatype, dest, sendtag,
					buf, count, datatype, source, recvtag,
					comm, status, err);
		} else {
			MPI_Fint ierr = MPI_SUCCESS;
			MPI_Fint packed_size;
			MPI_Fint info_null = MPI_Info_c2f(MPI_INFO_NULL);
			MPI_Fint recv_dt = MPI_Type_c2f(MPI_BYTE);
			void *helperbuf = NULL;

			/* setup a buffer for recv */
			mpi_pack_size_(count, datatype, comm, &packed_size, &ierr);
			if (ierr != MPI_SUCCESS)
				return;

			mpi_alloc_mem_(&packed_size, &info_null, &helperbuf, &ierr);
			if (ierr != MPI_SUCCESS)
				return;

			/* recv into temporary buffer */
			mpi_sendrecv_(buf, count, datatype, dest, sendtag,
					helperbuf, &packed_size, &recv_dt, source, recvtag,
					comm, status, &ierr);

			/* copy into users buffer */
			if (ierr == MPI_SUCCESS) {
				int position = 0;
				mpi_unpack_(helperbuf, &packed_size, &position, buf, count, datatype, comm, &ierr);
			}

			/* release resources */
			if (ierr == MPI_SUCCESS) {
				mpi_free_mem_(helperbuf, err);
			} else {
				mpi_free_mem_(helperbuf, &ierr);
			}
		}
	} else {
		static mpi_sendrecv_replace_t *symbol = (mpi_sendrecv_replace_t *) Symbol::load(__func__);
		(*symbol)(buf, count, datatype, dest, sendtag, source, recvtag, comm, status, err);
	}
}

} // extern C

#pragma GCC visibility pop
