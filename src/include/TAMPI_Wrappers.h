/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef TAMPI_WRAPPERS_H
#define TAMPI_WRAPPERS_H

#include <mpi.h>

#include "TAMPI_Decl.h"

#pragma GCC visibility push(default)

//! Read only buffers must be defined as const in versions MPI 3.0 and later
#if MPI_VERSION >= 3
	#define __MPI3CONST const
#else
	#define __MPI3CONST
#endif

#ifdef __cplusplus
extern "C" {
#endif

int TAMPI_Ibsend(__MPI3CONST void *buf, int count, MPI_Datatype datatype,
		int dest, int tag, MPI_Comm comm);

int TAMPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source,
		int tag, MPI_Comm comm, MPI_Status *status);

int TAMPI_Irsend(__MPI3CONST void *buf, int count, MPI_Datatype datatype,
		int source, int tag, MPI_Comm comm);

int TAMPI_Isend(__MPI3CONST void *buf, int count, MPI_Datatype datatype,
		int dest, int tag, MPI_Comm comm);

int TAMPI_Issend(__MPI3CONST void *buf, int count, MPI_Datatype datatype,
		int source, int tag, MPI_Comm comm);

int TAMPI_Iallgather(__MPI3CONST void *sendbuf, int sendcount,
		MPI_Datatype sendtype, void *recvbuf, int recvcount,
		MPI_Datatype recvtype, MPI_Comm comm);

int TAMPI_Iallgatherv(__MPI3CONST void *sendbuf, int sendcount,
		MPI_Datatype sendtype, void *recvbuf, __MPI3CONST int recvcounts[],
		__MPI3CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm);

int TAMPI_Iallreduce(__MPI3CONST void *sendbuf, void *recvbuf, int count,
		MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);

int TAMPI_Ialltoall(__MPI3CONST void *sendbuf, int sendcount,
		MPI_Datatype sendtype, void *recvbuf, int recvcount,
		MPI_Datatype recvtype, MPI_Comm comm);

int TAMPI_Ialltoallv(__MPI3CONST void *sendbuf, __MPI3CONST int sendcounts[],
		__MPI3CONST int sdispls[], MPI_Datatype sendtype, void *recvbuf,
		__MPI3CONST int recvcounts[], __MPI3CONST int rdispls[],
		MPI_Datatype recvtype, MPI_Comm comm);

int TAMPI_Ialltoallw(__MPI3CONST void *sendbuf, __MPI3CONST int sendcounts[],
		__MPI3CONST int sdispls[], __MPI3CONST MPI_Datatype sendtypes[],
		void *recvbuf, __MPI3CONST int recvcounts[], __MPI3CONST int rdispls[],
		__MPI3CONST MPI_Datatype recvtypes[], MPI_Comm comm);

int TAMPI_Ibarrier(MPI_Comm comm);

int TAMPI_Ibcast(void *buf, int count, MPI_Datatype datatype, int root,
		MPI_Comm comm);

int TAMPI_Igather(__MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);

int TAMPI_Igatherv(__MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, __MPI3CONST int recvcounts[], __MPI3CONST int displs[],
		MPI_Datatype recvtype, int root, MPI_Comm comm);

int TAMPI_Ireduce(__MPI3CONST void *sendbuf, void *recvbuf, int count,
		MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);

int TAMPI_Ireduce_scatter(__MPI3CONST void *sendbuf, void *recvbuf,
		__MPI3CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op,
		MPI_Comm comm);

int TAMPI_Ireduce_scatter_block(__MPI3CONST void *sendbuf, void *recvbuf,
		int recvcount, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);

int TAMPI_Iscatter(__MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);

int TAMPI_Iscatterv(__MPI3CONST void *sendbuf, __MPI3CONST int sendcounts[],
		__MPI3CONST int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount,
		MPI_Datatype recvtype, int root, MPI_Comm comm);

int TAMPI_Iscan(__MPI3CONST void *sendbuf, void *recvbuf, int count,
		MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);

int TAMPI_Iexscan(__MPI3CONST void *sendbuf, void *recvbuf, int count,
		MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);

#ifdef __cplusplus
}
#endif

#pragma GCC visibility pop

#endif /* TAMPI_WRAPPERS_H */
