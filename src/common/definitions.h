/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <mpi.h>

//! Read only buffers must be defined as const in versions MPI 3.0 and later.
#if MPI_VERSION >= 3
	#define MPI3CONST const
#else
	#define MPI3CONST
#endif

#endif // DEFINITIONS_H
