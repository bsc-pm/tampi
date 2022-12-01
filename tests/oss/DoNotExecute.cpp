/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2019-2022 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>
#include <TAMPI.h>

#include "Utils.hpp"

//! NOTE: Do not take this test as an example, since
//! it is not correct. It is only useful to check that
//! compiles properly. Please do not execute it.

int main(int argc, char **argv)
{
	fprintf(stderr, "This code should not be executed!\n");
	if (argc > 0) {
		return 1;
	}

	int provided;
	const int required = MPI_TASK_MULTIPLE;
	CHECK(MPI_Init_thread(&argc, &argv, required, &provided));
	ASSERT(provided == required);

	int rank, size;
	CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
	CHECK(MPI_Comm_size(MPI_COMM_WORLD, &size));

	void *sendbuf, *recvbuf;
	MPI_Request request;
	MPI_Status status;
	MPI_Datatype type;
	int count, *aux;
	MPI_Comm comm;
	MPI_Op op;

	TAMPI_Ibsend(sendbuf, count, type, count, count, comm, &request);
	TAMPI_Irecv(recvbuf, count, type, count, count, comm, &request, &status);
	TAMPI_Irsend(sendbuf, count, type, count, count, comm, &request);
	TAMPI_Isend(sendbuf, count, type, count, count, comm, &request);
	TAMPI_Issend(sendbuf, count, type, count, count, comm, &request);
	TAMPI_Iwait(&request, &status);
	TAMPI_Iwaitall(count, &request, &status);

	TAMPI_Iallgather(sendbuf, count, type, recvbuf, count, type, comm, &request);
	TAMPI_Iallgatherv(sendbuf, count, type, recvbuf, aux, aux, type, comm, &request);
	TAMPI_Iallreduce(sendbuf, recvbuf, count, type, op, comm, &request);
	TAMPI_Ialltoall(sendbuf, count, type, recvbuf, count, type, comm, &request);
	TAMPI_Ialltoallv(sendbuf, aux, aux, type, recvbuf, aux, aux, type, comm, &request);
	TAMPI_Ialltoallw(sendbuf, aux, aux, &type, recvbuf, aux, aux, &type, comm, &request);
	TAMPI_Ibarrier(comm, &request);
	TAMPI_Ibcast(recvbuf, count, type, count, comm, &request);
	TAMPI_Igather(sendbuf, count, type, recvbuf, count, type, count, comm, &request);
	TAMPI_Igatherv(sendbuf, count, type, recvbuf, aux, aux, type, count, comm, &request);
	TAMPI_Ireduce(sendbuf, recvbuf, count, type, op, count, comm, &request);
	TAMPI_Ireduce_scatter(sendbuf, recvbuf, aux, type, op, comm, &request);
	TAMPI_Ireduce_scatter_block(sendbuf, recvbuf, count, type, op, comm, &request);
	TAMPI_Iscatter(sendbuf, count, type, recvbuf, count, type, count, comm, &request);
	TAMPI_Iscatterv(sendbuf, aux, aux, type, recvbuf, count, type, count, comm, &request);
	TAMPI_Iscan(sendbuf, recvbuf, count, type, op, comm, &request);
	TAMPI_Iexscan(sendbuf, recvbuf, count, type, op, comm, &request);
	TAMPI_Wait(&request, &status);
	TAMPI_Waitall(count, &request, &status);

	CHECK(MPI_Finalize());

	return 0;
}
