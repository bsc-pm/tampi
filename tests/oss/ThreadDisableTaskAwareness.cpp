/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>
#include <sched.h>
#include <TAMPI.h>
#include <unistd.h>

#include "Utils.hpp"

#define NBARRIERS 100

int rank, nranks;

void performBarriers(int num)
{
	// The OmpSs-2 tasks should run in an exclusive CPU. The task cannot change
	// to a different CPU if there is no scheduling point. This tests checks that
	// the task does not run any scheduling point. Take into account that this
	// test can produce false positives, e.g., when having a single core per rank
	const int cpu = sched_getcpu();

	for (size_t b = 0; b < num; ++b) {
		// Produce some artificial desynchronization among ranks
		if (rank == 0)
			usleep(1000);

		// These communications should not be task-awareness
		CHECK(MPI_Barrier(MPI_COMM_WORLD));

		// Check that the task did not change to another CPU
		ASSERT(cpu == sched_getcpu());
	}
}

int main(int argc, char **argv)
{
	int provided, value;

	// Check the thread task-awareness is enabled by default
	CHECK(TAMPI_Property_get(TAMPI_PROPERTY_THREAD_TASKAWARE, &value));
	ASSERT(value == 1);

	// Initialize MPI and TAMPI
	CHECK(MPI_Init_thread(&argc, &argv, MPI_TASK_MULTIPLE, &provided));
	ASSERT(provided == MPI_TASK_MULTIPLE);

	CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
	CHECK(MPI_Comm_size(MPI_COMM_WORLD, &nranks));

	if (nranks < 2) {
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	// Disable the task-awareness for this thread
	CHECK(TAMPI_Property_set(TAMPI_PROPERTY_THREAD_TASKAWARE, 0));

	// Perform several barriers without task-awareness
	performBarriers(NBARRIERS);

	// Restore the task-awareness for this thread
	CHECK(TAMPI_Property_set(TAMPI_PROPERTY_THREAD_TASKAWARE, 1));

	// Check the thread task-awareness is still enabled
	CHECK(TAMPI_Property_get(TAMPI_PROPERTY_THREAD_TASKAWARE, &value));
	ASSERT(value == 1);

	// Finalize MPI and TAMPI
	CHECK(MPI_Finalize());

	return 0;
}
