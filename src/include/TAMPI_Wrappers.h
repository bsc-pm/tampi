/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef TAMPI_WRAPPERS_H
#define TAMPI_WRAPPERS_H

#include <mpi.h>

#include "TAMPI_Decl.h"

#pragma GCC visibility push(default)

#ifdef __cplusplus
extern "C" {
#endif

int TAMPI_Ibsend(const void *buf, int count, MPI_Datatype datatype,
		int dest, int tag, MPI_Comm comm);

int TAMPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source,
		int tag, MPI_Comm comm, MPI_Status *status);

int TAMPI_Irsend(const void *buf, int count, MPI_Datatype datatype,
		int source, int tag, MPI_Comm comm);

int TAMPI_Isend(const void *buf, int count, MPI_Datatype datatype,
		int dest, int tag, MPI_Comm comm);

int TAMPI_Issend(const void *buf, int count, MPI_Datatype datatype,
		int source, int tag, MPI_Comm comm);

int TAMPI_Iallgather(const void *sendbuf, int sendcount,
		MPI_Datatype sendtype, void *recvbuf, int recvcount,
		MPI_Datatype recvtype, MPI_Comm comm);

int TAMPI_Iallgatherv(const void *sendbuf, int sendcount,
		MPI_Datatype sendtype, void *recvbuf, const int recvcounts[],
		const int displs[], MPI_Datatype recvtype, MPI_Comm comm);

int TAMPI_Iallreduce(const void *sendbuf, void *recvbuf, int count,
		MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);

int TAMPI_Ialltoall(const void *sendbuf, int sendcount,
		MPI_Datatype sendtype, void *recvbuf, int recvcount,
		MPI_Datatype recvtype, MPI_Comm comm);

int TAMPI_Ialltoallv(const void *sendbuf, const int sendcounts[],
		const int sdispls[], MPI_Datatype sendtype, void *recvbuf,
		const int recvcounts[], const int rdispls[],
		MPI_Datatype recvtype, MPI_Comm comm);

int TAMPI_Ialltoallw(const void *sendbuf, const int sendcounts[],
		const int sdispls[], const MPI_Datatype sendtypes[],
		void *recvbuf, const int recvcounts[], const int rdispls[],
		const MPI_Datatype recvtypes[], MPI_Comm comm);

int TAMPI_Ibarrier(MPI_Comm comm);

int TAMPI_Ibcast(void *buf, int count, MPI_Datatype datatype, int root,
		MPI_Comm comm);

int TAMPI_Igather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);

int TAMPI_Igatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, const int recvcounts[], const int displs[],
		MPI_Datatype recvtype, int root, MPI_Comm comm);

int TAMPI_Ireduce(const void *sendbuf, void *recvbuf, int count,
		MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);

int TAMPI_Ireduce_scatter(const void *sendbuf, void *recvbuf,
		const int recvcounts[], MPI_Datatype datatype, MPI_Op op,
		MPI_Comm comm);

int TAMPI_Ireduce_scatter_block(const void *sendbuf, void *recvbuf,
		int recvcount, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);

int TAMPI_Iscatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm);

int TAMPI_Iscatterv(const void *sendbuf, const int sendcounts[],
		const int displs[], MPI_Datatype sendtype, void *recvbuf, int recvcount,
		MPI_Datatype recvtype, int root, MPI_Comm comm);

int TAMPI_Iscan(const void *sendbuf, void *recvbuf, int count,
		MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);

int TAMPI_Iexscan(const void *sendbuf, void *recvbuf, int count,
		MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);

#ifdef __cplusplus
}
#endif

#pragma GCC visibility pop

#endif /* TAMPI_WRAPPERS_H */
