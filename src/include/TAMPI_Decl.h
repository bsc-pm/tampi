/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef TAMPI_DECL_H
#define TAMPI_DECL_H

#include <mpi.h>

#pragma GCC visibility push(default)

#ifdef __cplusplus
extern "C" {
#endif

//! The MPI thread level that enables the task-aware blocking mode
#define MPI_TASK_MULTIPLE (MPI_THREAD_MULTIPLE + 1)

//! Functions to know which TAMPI modes are enabled
int TAMPI_Blocking_enabled(int *flag);
int TAMPI_Nonblocking_enabled(int *flag);

//! Functions to asycnhronously wait communications from tasks
int TAMPI_Iwait(MPI_Request *request, MPI_Status *status);
int TAMPI_Iwaitall(int count, MPI_Request requests[], MPI_Status statuses[]);

//! Fortran prototypes of previous C/C++ functions
void tampi_blocking_enabled_(MPI_Fint *flag, MPI_Fint *err);
void tampi_nonblocking_enabled_(MPI_Fint *flag, MPI_Fint *err);
void tampi_iwait_(MPI_Fint *request, MPI_Fint *status, MPI_Fint *err);
void tampi_iwaitall_(MPI_Fint *count, MPI_Fint requests[], MPI_Fint *statuses, MPI_Fint *err);

#ifdef __cplusplus
}
#endif

#pragma GCC visibility pop

#endif /* TAMPI_DECL_H */
