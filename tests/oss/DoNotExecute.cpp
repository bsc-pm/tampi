/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2019-2024 Barcelona Supercomputing Center (BSC)
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
	MPI_Init_thread(&argc, &argv, required, &provided);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	void *sendbuf, *recvbuf;
	MPI_Request request;
	MPI_Status status;
	MPI_Datatype type;
	int count, *aux;
	MPI_Comm comm;
	MPI_Op op;

	TAMPI_Ibsend(sendbuf, count, type, count, count, comm);
	TAMPI_Irecv(recvbuf, count, type, count, count, comm, &status);
	TAMPI_Irsend(sendbuf, count, type, count, count, comm);
	TAMPI_Isend(sendbuf, count, type, count, count, comm);
	TAMPI_Issend(sendbuf, count, type, count, count, comm);
	TAMPI_Iwait(&request, &status);
	TAMPI_Iwaitall(count, &request, &status);

	TAMPI_Iallgather(sendbuf, count, type, recvbuf, count, type, comm);
	TAMPI_Iallgatherv(sendbuf, count, type, recvbuf, aux, aux, type, comm);
	TAMPI_Iallreduce(sendbuf, recvbuf, count, type, op, comm);
	TAMPI_Ialltoall(sendbuf, count, type, recvbuf, count, type, comm);
	TAMPI_Ialltoallv(sendbuf, aux, aux, type, recvbuf, aux, aux, type, comm);
	TAMPI_Ialltoallw(sendbuf, aux, aux, &type, recvbuf, aux, aux, &type, comm);
	TAMPI_Ibarrier(comm);
	TAMPI_Ibcast(recvbuf, count, type, count, comm);
	TAMPI_Igather(sendbuf, count, type, recvbuf, count, type, count, comm);
	TAMPI_Igatherv(sendbuf, count, type, recvbuf, aux, aux, type, count, comm);
	TAMPI_Ireduce(sendbuf, recvbuf, count, type, op, count, comm);
	TAMPI_Ireduce_scatter(sendbuf, recvbuf, aux, type, op, comm);
	TAMPI_Ireduce_scatter_block(sendbuf, recvbuf, count, type, op, comm);
	TAMPI_Iscatter(sendbuf, count, type, recvbuf, count, type, count, comm);
	TAMPI_Iscatterv(sendbuf, aux, aux, type, recvbuf, count, type, count, comm);
	TAMPI_Iscan(sendbuf, recvbuf, count, type, op, comm);
	TAMPI_Iexscan(sendbuf, recvbuf, count, type, op, comm);

	MPI_Finalize();

	return 0;
}
