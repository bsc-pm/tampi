/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef TAMPI_DECL_H
#define TAMPI_DECL_H

#include <mpi.h>

#pragma GCC visibility push(default)

#ifdef __cplusplus
extern "C" {
#endif

//! Constant defining the major version of the current interface
#define TAMPI_VERSION_MAJOR 4

//! Constant defining the minor version of the current interface
#define TAMPI_VERSION_MINOR 0

//! The MPI thread level that enables the task-aware blocking mode
#define MPI_TASK_MULTIPLE (MPI_THREAD_MULTIPLE + 1)

//! The TAMPI_PROPERTY_BLOCKING_MODE is the property to obtain whether the task-
//! aware blocking mode is enabled. This property can only be consulted; setting
//! its value is invalid
#define TAMPI_PROPERTY_BLOCKING_MODE 0x1

//! The TAMPI_PROPERTY_NONBLOCKING_MODE is the property to obtain whether the
//! task-aware non-blocking mode is enabled. This property can only consulted;
//! setting its value is invalid
#define TAMPI_PROPERTY_NONBLOCKING_MODE 0x2

//! The TAMPI_PROPERTY_AUTO_INIT enables/disables the automatic initialization
//! and finalization of TAMPI when MPI_Init, MPI_Init_thread and MPI_Finalized
//! are called. The property value is treated as a boolean that enables or
//! disables the automatic initialization. Changing this property is only valid
//! before MPI_Init is called. By default, the TAMPI is automatically initialized
//! and finalizes when MPI does
#define TAMPI_PROPERTY_AUTO_INIT 0x3

//! The TAMPI_PROPERTY_THREAD_TASKAWARE enables/disables the task-awareness for
//! the calling thread. The task-awareness is the interception of blocking MPI
//! calls and the pause/resume of communication tasks. This property is per
//! thread and only considered when the MPI_TASK_MULTIPLE is enabled. It is user
//! responsibility to enable and disable it whenever it is necessary. When a
//! thread encounters a blocking MPI call, it will check this thread-local value
//! to decide whether the communication must be task-aware or not. By default,
//! all threads have their task-awareness enabled
#define TAMPI_PROPERTY_THREAD_TASKAWARE 0x4

//! Functions to get and set library properties
int TAMPI_Property_get(int property, int *value);
int TAMPI_Property_set(int property, int value);

//! Functions to initialize and finalize explicitly
int TAMPI_Init(int required, int *provided);
int TAMPI_Finalize(void);

//! Functions to know which TAMPI modes are enabled
int TAMPI_Blocking_enabled(int *flag);
int TAMPI_Nonblocking_enabled(int *flag);

//! Functions to asycnhronously wait communications from tasks
int TAMPI_Iwait(MPI_Request *request, MPI_Status *status);
int TAMPI_Iwaitall(int count, MPI_Request requests[], MPI_Status statuses[]);

//! Fortran prototypes of previous C/C++ functions
void tampi_property_get_(MPI_Fint *property, MPI_Fint *value, MPI_Fint *err);
void tampi_property_set_(MPI_Fint *property, MPI_Fint *value, MPI_Fint *err);
void tampi_init_(MPI_Fint *required, MPI_Fint *provided, MPI_Fint *err);
void tampi_finalize_(MPI_Fint *err);
void tampi_blocking_enabled_(MPI_Fint *flag, MPI_Fint *err);
void tampi_nonblocking_enabled_(MPI_Fint *flag, MPI_Fint *err);
void tampi_iwait_(MPI_Fint *request, MPI_Fint *status, MPI_Fint *err);
void tampi_iwaitall_(MPI_Fint *count, MPI_Fint requests[], MPI_Fint *statuses, MPI_Fint *err);

#ifdef __cplusplus
}
#endif

#pragma GCC visibility pop

#endif /* TAMPI_DECL_H */
