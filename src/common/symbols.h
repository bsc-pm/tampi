/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <dlfcn.h>
#include <mpi.h>
#include <string>

#include "definitions.h"
#include "error.h"

// MPI Test fortran specializations
extern "C" {
void pmpi_request_get_status_(MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
void pmpi_test_(MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
void pmpi_testall_(MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[], MPI_Fint*);
void pmpi_testany_(MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
void pmpi_testsome_(MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[], MPI_Fint[], MPI_Fint*);
} // extern C

namespace C {
typedef int MPI_Finalize_t(void);
typedef int MPI_Init_t(int*, char***);
typedef int MPI_Init_thread_t(int*, char***, int, int*);
typedef int MPI_Recv_t(void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Status*);
typedef int MPI_Send_t(MPI3CONST void*, int, MPI_Datatype, int, int, MPI_Comm);
typedef int MPI_Sendrecv_t(MPI3CONST void*, int, MPI_Datatype, int, int, void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Status*);
typedef int MPI_Sendrecv_replace_t(void*, int, MPI_Datatype, int, int, int, int, MPI_Comm, MPI_Status*);
typedef int MPI_Ssend_t(MPI3CONST void*, int, MPI_Datatype, int, int, MPI_Comm);
typedef int MPI_Wait_t(MPI_Request*, MPI_Status*);
typedef int MPI_Waitall_t(int, MPI_Request[], MPI_Status[]);

#if MPI_VERSION >=3
typedef int MPI_Allgatherv_t(const void*, int, MPI_Datatype, void*, const int[], const int[], MPI_Datatype, MPI_Comm);
typedef int MPI_Allreduce_t(const void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm);
typedef int MPI_Alltoall_t(const void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm);
typedef int MPI_Alltoallv_t(const void*, const int[], const int[], MPI_Datatype, void*, const int[], const int[], MPI_Datatype, MPI_Comm);
typedef int MPI_Barrier_t(MPI_Comm);
typedef int	MPI_Bcast_t(void*, int, MPI_Datatype, int, MPI_Comm);
typedef int MPI_Gatherv_t(const void*, int, MPI_Datatype, void*, const int[], const int[], MPI_Datatype, int, MPI_Comm);
typedef int MPI_Reduce_t(const void*, void*, int, MPI_Datatype, MPI_Op, int, MPI_Comm);
#endif // MPI_VERSION
} // namespace C

namespace Fortran {
typedef void mpi_finalize_t(MPI_Fint*);
typedef void mpi_init_t(MPI_Fint*);
typedef void mpi_init_thread_t(MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_recv_t(void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_send_t(MPI3CONST void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_sendrecv_t(MPI3CONST void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_sendrecv_replace_t(void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_ssend_t(MPI3CONST void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_wait_t(MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_waitall_t(MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint*);

#if MPI_VERSION >=3
typedef void mpi_allgather_t(const void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_allgatherv_t(const void*, MPI_Fint*, MPI_Fint*, void*, const MPI_Fint[], const MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_allreduce_t(const void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_alltoall_t(const void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_alltoallv_t(const void*, const MPI_Fint[], const MPI_Fint[], MPI_Fint*, void*, const MPI_Fint[], const MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_barrier_t(MPI_Fint*, MPI_Fint*);
typedef void mpi_bcast_t(void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_gatherv_t(const void*, MPI_Fint*, MPI_Fint*, void*, const MPI_Fint[], const MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_reduce_t(const void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_scatterv_t(const void*, const MPI_Fint[], const MPI_Fint[], MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
#endif // MPI_VERSION
} // namespace Fortran

class Symbol {
public:
	static inline void *loadNextSymbol(const std::string &symbolName)
	{
		void *symbol = dlsym(RTLD_NEXT, symbolName.c_str());
		Error::failIf(!symbol, symbolName + " symbol could not be loaded!");
		return symbol;
	}
};

#endif // SYMBOLS_H
