/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2019-2024 Barcelona Supercomputing Center (BSC)
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

const int TOTAL_SIZE = MSG_NUM * MSG_SIZE;

MPI_Status statuses[MSG_NUM];

int main(int argc, char **argv)
{
	int provided;
	const int required = MPI_TASK_MULTIPLE;
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

	for (int t = 0; t < TIMESTEPS; ++t) {
		if (rank == 0) {
			#pragma oss task out(buffer[0;TOTAL_SIZE]) label("init")
			for (int d = 0; d < TOTAL_SIZE; ++d) {
				buffer[d] = d;
			}

			#pragma oss task in(buffer[0;TOTAL_SIZE]) label("send")
			{
				for (int m = 0; m < MSG_NUM; ++m) {
					CHECK(TAMPI_Isend(&buffer[m*MSG_SIZE], MSG_SIZE, MPI_INT, 1, m, MPI_COMM_WORLD));
				}
			}
		} else if (rank == 1) {
			int *message = buffer + (MSG_NUM - 1) * MSG_SIZE;

			#pragma oss task out(buffer[0;TOTAL_SIZE]) shared(statuses) label("recv")
			{
				for (int m = MSG_NUM - 1; m >= 0; --m) {
					CHECK(MPI_Recv(&buffer[m*MSG_SIZE], MSG_SIZE, MPI_INT, 0, m, MPI_COMM_WORLD, &statuses[MSG_NUM-1-m]));
				}

				for (int m = 0; m < MSG_NUM; ++m) {
					ASSERT(statuses[m].MPI_TAG == MSG_NUM - 1 - m);
					ASSERT(statuses[m].MPI_SOURCE == 0);

					int count;
					CHECK(MPI_Get_count(&statuses[m], MPI_INT, &count));
					ASSERT(count == MSG_SIZE);
				}
			}

			#pragma oss task in(buffer[0;TOTAL_SIZE]) label("check")
			for (int d = 0; d < TOTAL_SIZE; ++d) {
				ASSERT(buffer[d] == d);
			}
		}
	}
	#pragma oss taskwait

	CHECK(MPI_Barrier(MPI_COMM_WORLD));

	if (rank == 0) {
		double endTime = getTime();
		fprintf(stdout, "Success, time: %f\n", endTime - startTime);
	}

	CHECK(MPI_Finalize());

	std::free(buffer);

	return 0;
}
