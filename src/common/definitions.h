#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <mpi.h>

#ifndef MPI_TASK_MULTIPLE
#define MPI_TASK_MULTIPLE (MPI_THREAD_MULTIPLE + 1)
#endif

//! Read only buffers must be defined as const in versions MPI 3.0 and later.
#if MPI_VERSION >= 3
	#define MPI3CONST const
#else
	#define MPI3CONST
#endif

#endif // DEFINITIONS_H
