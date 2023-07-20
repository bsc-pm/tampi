/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>
#include <TAMPI.h>

#include "Utils.hpp"

void taskbody()
{
	int value;

	// Check the thread task-awareness is enabled by default
	CHECK(TAMPI_Property_get(TAMPI_PROPERTY_THREAD_TASKAWARE, &value));
	ASSERT(value == 1);

	// Disable the current thread task-awareness
	CHECK(TAMPI_Property_set(TAMPI_PROPERTY_THREAD_TASKAWARE, 0));

	// Check the thread task-awareness is disabled now
	CHECK(TAMPI_Property_get(TAMPI_PROPERTY_THREAD_TASKAWARE, &value));
	ASSERT(value == 0);

	// Disable the current thread task-awareness
	CHECK(TAMPI_Property_set(TAMPI_PROPERTY_THREAD_TASKAWARE, 1));

	// Check the thread task-awareness is enabled again
	CHECK(TAMPI_Property_get(TAMPI_PROPERTY_THREAD_TASKAWARE, &value));
	ASSERT(value == 1);
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

	CHECK(MPI_Barrier(MPI_COMM_WORLD));

	#pragma oss task
	taskbody();

	#pragma oss taskwait

	CHECK(MPI_Barrier(MPI_COMM_WORLD));

	// Check the thread task-awareness is still enabled
	CHECK(TAMPI_Property_get(TAMPI_PROPERTY_THREAD_TASKAWARE, &value));
	ASSERT(value == 1);

	// Finalize MPI and TAMPI
	CHECK(MPI_Finalize());

	return 0;
}
