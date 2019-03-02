/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
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

static inline int TAMPI_Ibsend(__MPI3CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Ibsend(buf, count, datatype, dest, tag, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request,
		__attribute__((unused)) MPI_Status *status)
{
	int err = MPI_Irecv(buf, count, datatype, source, tag, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, status);
#endif
	return err;
}

static inline int TAMPI_Irsend(__MPI3CONST void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Irsend(buf, count, datatype, source, tag, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Isend(__MPI3CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Isend(buf, count, datatype, dest, tag, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Issend(__MPI3CONST void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Issend(buf, count, datatype, source, tag, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Iallgather(__MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype,
		MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Iallgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Iallgatherv(__MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, __MPI3CONST int recvcounts[],
		__MPI3CONST int displs[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Iallgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Iallreduce(__MPI3CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm,
		MPI_Request *request)
{
	int err = MPI_Iallreduce(sendbuf, recvbuf, count, datatype, op, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Ialltoall(__MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount,
		MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Ialltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Ialltoallv(__MPI3CONST void *sendbuf, __MPI3CONST int sendcounts[], __MPI3CONST int sdispls[], MPI_Datatype sendtype,
		void *recvbuf, __MPI3CONST int recvcounts[], __MPI3CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Ialltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Ialltoallw(__MPI3CONST void *sendbuf, __MPI3CONST int sendcounts[], __MPI3CONST int sdispls[],
		__MPI3CONST MPI_Datatype sendtypes[], void *recvbuf, __MPI3CONST int recvcounts[], __MPI3CONST int rdispls[],
		__MPI3CONST MPI_Datatype recvtypes[], MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Ialltoallw(sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Ibarrier(MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Ibarrier(comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Ibcast(void *buf, int count, MPI_Datatype datatype, int root, MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Ibcast(buf, count, datatype, root, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Igather(__MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount,
		MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Igather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Igatherv(__MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf,
		__MPI3CONST int recvcounts[], __MPI3CONST int displs[], MPI_Datatype recvtype, int root, MPI_Comm comm,
		MPI_Request *request)
{
	int err = MPI_Igatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Ireduce(__MPI3CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root,
		MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Ireduce(sendbuf, recvbuf, count, datatype, op, root, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Ireduce_scatter(__MPI3CONST void *sendbuf, void *recvbuf, __MPI3CONST int recvcounts[], MPI_Datatype datatype,
		MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Ireduce_scatter_block(__MPI3CONST void *sendbuf, void *recvbuf, int recvcount, MPI_Datatype datatype, MPI_Op op,
		MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Ireduce_scatter_block(sendbuf, recvbuf, recvcount, datatype, op, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Iscatter(__MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount,
		MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Iscatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Iscatterv(__MPI3CONST void *sendbuf, __MPI3CONST int sendcounts[], __MPI3CONST int displs[], MPI_Datatype sendtype,
		void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Iscatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Iscan(__MPI3CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Iscan(sendbuf, recvbuf, count, datatype, op, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Iexscan(__MPI3CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm, MPI_Request *request)
{
	int err = MPI_Iexscan(sendbuf, recvbuf, count, datatype, op, comm, request);
#if defined(_OMPSS_2) || defined(_OPENMP)
	if (err == MPI_SUCCESS)
		err = TAMPI_Iwait(request, MPI_STATUS_IGNORE);
#endif
	return err;
}

static inline int TAMPI_Wait(MPI_Request *request, MPI_Status *status)
{
#if defined(_OMPSS_2) || defined(_OPENMP)
	int enabled = 0;
	TAMPI_Nonblocking_enabled(&enabled);
	if (enabled) {
		return MPI_SUCCESS;
	}
#endif
	return MPI_Wait(request, status);
}

static inline int TAMPI_Waitall(int count, MPI_Request requests[], MPI_Status statuses[])
{
#if defined(_OMPSS_2) || defined(_OPENMP)
	int enabled = 0;
	TAMPI_Nonblocking_enabled(&enabled);
	if (enabled) {
		return MPI_SUCCESS;
	}
#endif
	return MPI_Waitall(count, requests, statuses);
}

#ifdef __cplusplus
}
#endif

#pragma GCC visibility pop

#endif /* TAMPI_WRAPPERS_H */
