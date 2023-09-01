/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef DECLARATIONS_HPP
#define DECLARATIONS_HPP

#include <mpi.h>

#include <Symbol.hpp>

//! Read only buffers must be defined as const in versions MPI 3.0 and later
#if MPI_VERSION >= 3
#define MPI3CONST const
#else
#define MPI3CONST
#endif

using namespace tampi;

//! Setup operations in C
using MPI_Finalize_t = SymbolDecl<int>;
using MPI_Init_t = SymbolDecl<int, int*, char***>;
using MPI_Init_thread_t = SymbolDecl<int, int*, char***, int, int*>;
using MPI_Query_thread_t = SymbolDecl<int, int*>;

//! Request waiting/testing operations in C
using MPI_Test_t = SymbolDecl<int, MPI_Request*, int*, MPI_Status*>;
using MPI_Testall_t = SymbolDecl<int, int, MPI_Request[], int*, MPI_Status[]>;
using MPI_Testsome_t = SymbolDecl<int, int, MPI_Request[], int*, int[], MPI_Status[]>;
using MPI_Wait_t = SymbolDecl<int, MPI_Request*, MPI_Status*>;
using MPI_Waitall_t = SymbolDecl<int, int, MPI_Request[], MPI_Status[]>;

//! Point-to-point blocking operations in C
using MPI_Bsend_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, int, int, MPI_Comm>;
using MPI_Recv_t = SymbolDecl<int, void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Status*>;
using MPI_Rsend_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, int, int, MPI_Comm>;
using MPI_Sendrecv_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, int, int, void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Status*>;
using MPI_Sendrecv_replace_t = SymbolDecl<int, void*, int, MPI_Datatype, int, int, int, int, MPI_Comm, MPI_Status*>;
using MPI_Ssend_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, int, int, MPI_Comm>;
using MPI_Send_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, int, int, MPI_Comm>;

//! Point-to-point non-blocking operations in C
using MPI_Ibsend_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*>;
using MPI_Irecv_t = SymbolDecl<int, void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*>;
using MPI_Irsend_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*>;
using MPI_Issend_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*>;
using MPI_Isend_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*>;

//! Collective blocking operations in C
using MPI_Allgather_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm>;
using MPI_Allgatherv_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, void*, MPI3CONST int[], MPI3CONST int[], MPI_Datatype, MPI_Comm>;
using MPI_Allreduce_t = SymbolDecl<int, MPI3CONST void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm>;
using MPI_Alltoall_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm>;
using MPI_Alltoallv_t = SymbolDecl<int, MPI3CONST void*, MPI3CONST int[], MPI3CONST int[], MPI_Datatype, void*, MPI3CONST int[], MPI3CONST int[], MPI_Datatype, MPI_Comm>;
using MPI_Alltoallw_t = SymbolDecl<int, MPI3CONST void*, MPI3CONST int[], MPI3CONST int[], MPI3CONST MPI_Datatype[], void*, MPI3CONST int[], MPI3CONST int[], MPI3CONST MPI_Datatype[], MPI_Comm>;
using MPI_Barrier_t = SymbolDecl<int, MPI_Comm>;
using MPI_Bcast_t = SymbolDecl<int, void*, int, MPI_Datatype, int, MPI_Comm>;
using MPI_Gather_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm>;
using MPI_Gatherv_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, void*, MPI3CONST int[], MPI3CONST int[], MPI_Datatype, int, MPI_Comm>;
using MPI_Reduce_t = SymbolDecl<int, MPI3CONST void*, void*, int, MPI_Datatype, MPI_Op, int, MPI_Comm>;
using MPI_Reduce_scatter_t = SymbolDecl<int, MPI3CONST void*, void*, MPI3CONST int[], MPI_Datatype, MPI_Op, MPI_Comm>;
using MPI_Reduce_scatter_block_t = SymbolDecl<int, MPI3CONST void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm>;
using MPI_Scatter_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm>;
using MPI_Scatterv_t = SymbolDecl<int, MPI3CONST void*, MPI3CONST int[], MPI3CONST int[], MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm>;
using MPI_Scan_t = SymbolDecl<int, MPI3CONST void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm>;
using MPI_Exscan_t = SymbolDecl<int, MPI3CONST void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm>;

//! Collective non-blocking operations in C
using MPI_Iallgather_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm, MPI_Request*>;
using MPI_Iallgatherv_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, void*, MPI3CONST int[], MPI3CONST int[], MPI_Datatype, MPI_Comm, MPI_Request*>;
using MPI_Iallreduce_t = SymbolDecl<int, MPI3CONST void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm, MPI_Request*>;
using MPI_Ialltoall_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm, MPI_Request*>;
using MPI_Ialltoallv_t = SymbolDecl<int, MPI3CONST void*, MPI3CONST int[], MPI3CONST int[], MPI_Datatype, void*, MPI3CONST int[], MPI3CONST int[], MPI_Datatype, MPI_Comm, MPI_Request*>;
using MPI_Ialltoallw_t = SymbolDecl<int, MPI3CONST void*, MPI3CONST int[], MPI3CONST int[], MPI3CONST MPI_Datatype[], void*, MPI3CONST int[], MPI3CONST int[], MPI3CONST MPI_Datatype[], MPI_Comm, MPI_Request*>;
using MPI_Ibarrier_t = SymbolDecl<int, MPI_Comm, MPI_Request*>;
using MPI_Ibcast_t = SymbolDecl<int, void*, int, MPI_Datatype, int, MPI_Comm, MPI_Request*>;
using MPI_Igather_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm, MPI_Request*>;
using MPI_Igatherv_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, void*, MPI3CONST int[], MPI3CONST int[], MPI_Datatype, int, MPI_Comm, MPI_Request*>;
using MPI_Ireduce_t = SymbolDecl<int, MPI3CONST void*, void*, int, MPI_Datatype, MPI_Op, int, MPI_Comm, MPI_Request*>;
using MPI_Ireduce_scatter_t = SymbolDecl<int, MPI3CONST void*, void*, MPI3CONST int[], MPI_Datatype, MPI_Op, MPI_Comm, MPI_Request*>;
using MPI_Ireduce_scatter_block_t = SymbolDecl<int, MPI3CONST void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm, MPI_Request*>;
using MPI_Iscatter_t = SymbolDecl<int, MPI3CONST void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm, MPI_Request*>;
using MPI_Iscatterv_t = SymbolDecl<int, MPI3CONST void*, MPI3CONST int[], MPI3CONST int[], MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm, MPI_Request*>;
using MPI_Iscan_t = SymbolDecl<int, MPI3CONST void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm, MPI_Request*>;
using MPI_Iexscan_t = SymbolDecl<int, MPI3CONST void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm, MPI_Request*>;

//! Setup operations in Fortran
using mpi_finalize_t = SymbolDecl<void, MPI_Fint*>;
using mpi_init_t = SymbolDecl<void, MPI_Fint*>;
using mpi_init_thread_t = SymbolDecl<void, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_query_thread_t = SymbolDecl<void, MPI_Fint*, MPI_Fint*>;

//! Request testing/waiting operations in Fortran
using mpi_test_t = SymbolDecl<void, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_testall_t = SymbolDecl<void, MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[], MPI_Fint*>;
using mpi_testsome_t = SymbolDecl<void, MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[], MPI_Fint[], MPI_Fint*>;
using mpi_wait_t = SymbolDecl<void, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_waitall_t = SymbolDecl<void, MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint*>;

//! Point-to-point blocking operations in Fortran
using mpi_bsend_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_recv_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_rsend_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_send_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_sendrecv_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_sendrecv_replace_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_ssend_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;

//! Point-to-point non-blocking operations in Fortran
using mpi_ibsend_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_irecv_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_irsend_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_isend_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_issend_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;

//! Collective blocking operations in Fortran
using mpi_allgather_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_allgatherv_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint[], MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_allreduce_t = SymbolDecl<void, void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_alltoall_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_alltoallv_t = SymbolDecl<void, void*, MPI_Fint[], MPI_Fint[], MPI_Fint*, void*, MPI_Fint[], MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_alltoallw_t = SymbolDecl<void, void*, MPI_Fint[], MPI_Fint[], MPI_Fint[], void*, MPI_Fint[], MPI_Fint[], MPI_Fint[], MPI_Fint*, MPI_Fint*>;
using mpi_barrier_t = SymbolDecl<void, MPI_Fint*, MPI_Fint*>;
using mpi_bcast_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_gather_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_gatherv_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint[], MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_reduce_t = SymbolDecl<void, void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_reduce_scatter_t = SymbolDecl<void, void*, void*, MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_reduce_scatter_block_t = SymbolDecl<void, void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_scatter_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_scatterv_t = SymbolDecl<void, void*, MPI_Fint[], MPI_Fint[], MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_scan_t = SymbolDecl<void, void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_exscan_t = SymbolDecl<void, void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;

//! Collective non-blocking operations in Fortran
using mpi_iallgather_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_iallgatherv_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint[], MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_iallreduce_t = SymbolDecl<void, void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_ialltoall_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_ialltoallv_t = SymbolDecl<void, void*, MPI_Fint[], MPI_Fint[], MPI_Fint*, void*, MPI_Fint[], MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_ialltoallw_t = SymbolDecl<void, void*, MPI_Fint[], MPI_Fint[], MPI_Fint[], void*, MPI_Fint[], MPI_Fint[], MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_ibarrier_t = SymbolDecl<void, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_ibcast_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_igather_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_igatherv_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint[], MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_ireduce_t = SymbolDecl<void, void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_ireduce_scatter_t = SymbolDecl<void, void*, void*, MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_ireduce_scatter_block_t = SymbolDecl<void, void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_iscatter_t = SymbolDecl<void, void*, MPI_Fint*, MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_iscatterv_t = SymbolDecl<void, void*, MPI_Fint[], MPI_Fint[], MPI_Fint*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_iscan_t = SymbolDecl<void, void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
using mpi_iexscan_t = SymbolDecl<void, void*, void*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;

extern "C" {

//! MPI Fortran declarations
void mpi_comm_rank_(MPI_Fint*, MPI_Fint*, MPI_Fint*);
void mpi_comm_size_(MPI_Fint*, MPI_Fint*, MPI_Fint*);
void mpi_query_thread_(MPI_Fint*, MPI_Fint*);
void mpi_test_(MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
void mpi_testall_(MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[], MPI_Fint*);
void mpi_testany_(MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*);
void mpi_testsome_(MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[], MPI_Fint[], MPI_Fint*);

} // extern C

#endif // DECLARATIONS_HPP
