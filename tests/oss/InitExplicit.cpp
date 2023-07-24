/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>
#include <TAMPI.h>

#include "Utils.hpp"

int main(int argc, char **argv)
{
	int provided, value;

	// Disable automatic TAMPI initialization
	CHECK(TAMPI_Property_set(TAMPI_PROPERTY_AUTO_INIT, 0));

	// Check the auto init is disabled now
	CHECK(TAMPI_Property_get(TAMPI_PROPERTY_AUTO_INIT, &value));
	ASSERT(value == 0);

	// Initialize MPI
	CHECK(MPI_Init(&argc, &argv));

	// Initialize TAMPI
	CHECK(TAMPI_Init(MPI_THREAD_SINGLE, &provided));
	ASSERT(provided == MPI_THREAD_SINGLE);

	CHECK(MPI_Barrier(MPI_COMM_WORLD));

	// Finalize TAMPI
	CHECK(TAMPI_Finalize());

	// Finalize MPI
	CHECK(MPI_Finalize());

	return 0;
}
