/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>
#include <TAMPI.h>

#include "Utils.hpp"

int main(int argc, char **argv)
{
	int value;

	// Check the auto init is enabled by default
	CHECK(TAMPI_Property_get(TAMPI_PROPERTY_AUTO_INIT, &value));
	ASSERT(value == 1);

	// Initialize MPI and TAMPI
	CHECK(MPI_Init(&argc, &argv));

	CHECK(MPI_Barrier(MPI_COMM_WORLD));

	// Finalize TAMPI and MPI
	CHECK(MPI_Finalize());

	return 0;
}
