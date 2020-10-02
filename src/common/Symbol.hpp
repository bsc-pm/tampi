/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2020 Barcelona Supercomputing Center (BSC)
*/

#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <dlfcn.h>
#include <string>

#include <mpi.h>

#include "Definitions.hpp"
#include "util/ErrorHandler.hpp"


namespace tampi {

//! Class that allows the dynamic loading of symbols at run-time
class Symbol {
public:
	//! \brief Load a symbol from the subsequent libraries
	//!
	//! \param symbolName The name of the symbol to load
	//! \param mandatory Whether should abort the program if not found
	//!
	//! \returns An opaque pointer to the symbol or null if not found
	static inline void *load(const std::string &symbolName, bool mandatory = true)
	{
		void *symbol = dlsym(RTLD_NEXT, symbolName.c_str());
		if (symbol == nullptr && mandatory) {
			ErrorHandler::fail("Could not find symbol ", symbolName);
		}
		return symbol;
	}
};

//! Prototypes of point-to-point operations in C
typedef int MPI_Bsend_t(MPI3CONST void*, int, MPI_Datatype, int, int, MPI_Comm);
typedef int MPI_Finalize_t(void);
typedef int MPI_Init_t(int*, char***);
typedef int MPI_Init_thread_t(int*, char***, int, int*);
typedef int MPI_Query_thread_t(int*);
typedef int MPI_Recv_t(void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Status*);
typedef int MPI_Rsend_t(MPI3CONST void*, int, MPI_Datatype, int, int, MPI_Comm);
typedef int MPI_Send_t(MPI3CONST void*, int, MPI_Datatype, int, int, MPI_Comm);
typedef int MPI_Sendrecv_t(MPI3CONST void*, int, MPI_Datatype, int, int, void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Status*);
typedef int MPI_Sendrecv_replace_t(void*, int, MPI_Datatype, int, int, int, int, MPI_Comm, MPI_Status*);
typedef int MPI_Ssend_t(MPI3CONST void*, int, MPI_Datatype, int, int, MPI_Comm);
typedef int MPI_Wait_t(MPI_Request*, MPI_Status*);
typedef int MPI_Waitall_t(int, MPI_Request[], MPI_Status[]);

//! Prototypes of collective operations in C
typedef int MPI_Allgather_t(MPI3CONST void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm);
typedef int MPI_Allgatherv_t(MPI3CONST void*, int, MPI_Datatype, void*, MPI3CONST int[], MPI3CONST int[], MPI_Datatype, MPI_Comm);
typedef int MPI_Allreduce_t(MPI3CONST void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm);
typedef int MPI_Alltoall_t(MPI3CONST void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm);
typedef int MPI_Alltoallv_t(MPI3CONST void*, MPI3CONST int[], MPI3CONST int[], MPI_Datatype, void*, MPI3CONST int[], MPI3CONST int[], MPI_Datatype, MPI_Comm);
typedef int MPI_Alltoallw_t(MPI3CONST void*, MPI3CONST int[], MPI3CONST int[], MPI3CONST MPI_Datatype[], void*, MPI3CONST int[], MPI3CONST int[], MPI3CONST MPI_Datatype[], MPI_Comm);
typedef int MPI_Barrier_t(MPI_Comm);
typedef int	MPI_Bcast_t(void*, int, MPI_Datatype, int, MPI_Comm);
typedef int MPI_Gather_t(MPI3CONST void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm);
typedef int MPI_Gatherv_t(MPI3CONST void*, int, MPI_Datatype, void*, MPI3CONST int[], MPI3CONST int[], MPI_Datatype, int, MPI_Comm);
typedef int MPI_Reduce_t(MPI3CONST void*, void*, int, MPI_Datatype, MPI_Op, int, MPI_Comm);
typedef int MPI_Reduce_scatter_t(MPI3CONST void*, void*, MPI3CONST int[], MPI_Datatype, MPI_Op, MPI_Comm);
typedef int MPI_Reduce_scatter_block_t(MPI3CONST void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm);
typedef int MPI_Scatter_t(MPI3CONST void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm);
typedef int MPI_Scatterv_t(MPI3CONST void*, MPI3CONST int[], MPI3CONST int[], MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm);
typedef int MPI_Scan_t(MPI3CONST void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm);
typedef int MPI_Exscan_t(MPI3CONST void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm);

//! Prototypes of point-to-point operations in Fortran
typedef void mpi_bsend_t(void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_finalize_t(MPI_Fint*);
typedef void mpi_init_t(MPI_Fint*);
typedef void mpi_init_thread_t(MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_query_thread_t(MPI_Fint*, MPI_Fint*);
typedef void mpi_recv_t(void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_rsend_t(void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_send_t(void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_sendrecv_t(void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_sendrecv_replace_t(void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_ssend_t(void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_wait_t(MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_waitall_t(MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint*);

//! Prototypes of collective operations in Fortran
typedef void mpi_allgather_t(void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_allgatherv_t(void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint[], MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_allreduce_t(void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_alltoall_t(void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_alltoallv_t(void*, MPI_Fint[], MPI_Fint[], MPI_Fint*, void*, MPI_Fint[], MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_alltoallw_t(void*, MPI_Fint[], MPI_Fint[], MPI_Fint[], void*, MPI_Fint[], MPI_Fint[], MPI_Fint[], MPI_Fint*, MPI_Fint*);
typedef void mpi_barrier_t(MPI_Fint*, MPI_Fint*);
typedef void mpi_bcast_t(void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_gather_t(void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_gatherv_t(void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint[], MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_reduce_t(void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_reduce_scatter_t(void*, void*, MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_reduce_scatter_block_t(void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_scatter_t(void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_scatterv_t(void*, MPI_Fint[], MPI_Fint[], MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_scan_t(void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
typedef void mpi_exscan_t(void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);

} // namespace tampi

extern "C" {

//! MPI Test fortran specializations
void pmpi_abort_(MPI_Fint *, MPI_Fint *, MPI_Fint *);
void pmpi_request_get_status_(MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
void pmpi_test_(MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
void pmpi_testall_(MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[], MPI_Fint*);
void pmpi_testany_(MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
void pmpi_testsome_(MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[], MPI_Fint[], MPI_Fint*);

} // extern C

#endif // SYMBOL_HPP
