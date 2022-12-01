/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2019-2022 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>
#include <TAMPI.h>

#include "Utils.hpp"

#include <vector>

#ifdef LARGE_INPUT
const int TIMESTEPS = 1000;
const int MSG_NUM = 1000;
const int MSG_SIZE = 100;
#else
const int TIMESTEPS = 500;
const int MSG_NUM = 500;
const int MSG_SIZE = 100;
#endif

MPI_Status statuses[MSG_NUM];

int main(int argc, char **argv)
{
	int provided;
	const int required = MPI_THREAD_MULTIPLE;
	CHECK(MPI_Init_thread(&argc, &argv, required, &provided));
	ASSERT(provided == required);

	int rank, size;
	CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
	CHECK(MPI_Comm_size(MPI_COMM_WORLD, &size));
	ASSERT(size > 1);

	int * const buffer = (int *) std::malloc(MSG_NUM * MSG_SIZE * sizeof(int));
	ASSERT(buffer != nullptr);

	CHECK(MPI_Barrier(MPI_COMM_WORLD));
	double startTime = getTime();

	#pragma omp parallel
	#pragma omp single
	{
		for (int t = 0; t < TIMESTEPS; ++t) {
			if (rank == 0) {
				int *message = buffer;

				for (int m = 0; m < MSG_NUM; ++m) {
					#pragma omp task depend(out: message[0:MSG_SIZE-1])
					for (int d = 0; d < MSG_SIZE; ++d) {
						message[d] = d;
					}

					#pragma omp task depend(in: message[0:MSG_SIZE-1])
					{
						MPI_Request request;
						CHECK(MPI_Isend(message, MSG_SIZE, MPI_INT, 1, m, MPI_COMM_WORLD, &request));
						CHECK(TAMPI_Iwait(&request, MPI_STATUS_IGNORE));
					}
					message += MSG_SIZE;
				}
			} else if (rank == 1) {
				int *message = buffer + (MSG_NUM - 1) * MSG_SIZE;

				for (int m = MSG_NUM - 1; m >= 0; --m) {
					#pragma omp task depend(out: message[0:MSG_SIZE-1], statuses[m])
					{
						MPI_Request request;
						CHECK(MPI_Irecv(message, MSG_SIZE, MPI_INT, 0, m, MPI_COMM_WORLD, &request));
						CHECK(TAMPI_Iwait(&request, &statuses[m]));
					}

					#pragma omp task depend(in: message[0:MSG_SIZE-1], statuses[m])
					for (int d = 0; d < MSG_SIZE; ++d) {
						ASSERT(message[d] == d);
						ASSERT(statuses[m].MPI_TAG == m);
						ASSERT(statuses[m].MPI_SOURCE == 0);

						int count;
						CHECK(MPI_Get_count(&statuses[m], MPI_INT, &count));
						ASSERT(count == MSG_SIZE);
					}
					message -= MSG_SIZE;
				}
			}
		}
		#pragma omp taskwait
	}

	CHECK(MPI_Barrier(MPI_COMM_WORLD));

	if (rank == 0) {
		double endTime = getTime();
		fprintf(stdout, "Success, time: %f\n", endTime - startTime);
	}

	CHECK(MPI_Finalize());

	std::free(buffer);

	return 0;
}
