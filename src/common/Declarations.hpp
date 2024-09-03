/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef DECLARATIONS_HPP
#define DECLARATIONS_HPP

#include <mpi.h>

#include <string_view>

#include <Symbol.hpp>

namespace tampi {

//! Classes representing C/C++ and Fortran languages. The MPI interface
//! is very different between the C/C++ and the Fortran launagues, so we
//! try to define common code and we use specific classes with the
//! differences, such as MPI functions and types.
struct C {};
struct Fortran {};

template <typename Lang>
struct Prototypes {};

template <>
struct Prototypes<C> {
	//! Setup operations in C
	using mpi_finalize_t = SymbolDecl<int>;
	using mpi_init_t = SymbolDecl<int, int*, char***>;
	using mpi_init_thread_t = SymbolDecl<int, int*, char***, int, int*>;
	using mpi_comm_rank_t = SymbolDecl<int, MPI_Comm, int*>;
	using mpi_comm_size_t = SymbolDecl<int, MPI_Comm, int*>;
	using mpi_query_thread_t = SymbolDecl<int, int*>;

	//! Request waiting/testing operations in C
	using mpi_test_t = SymbolDecl<int, MPI_Request*, int*, MPI_Status*>;
	using mpi_testall_t = SymbolDecl<int, int, MPI_Request[], int*, MPI_Status[]>;
	using mpi_testany_t = SymbolDecl<int, int, MPI_Request[], int*, int*, MPI_Status*>;
	using mpi_testsome_t = SymbolDecl<int, int, MPI_Request[], int*, int[], MPI_Status[]>;
	using mpi_wait_t = SymbolDecl<int, MPI_Request*, MPI_Status*>;
	using mpi_waitall_t = SymbolDecl<int, int, MPI_Request[], MPI_Status[]>;

	//! Point-to-point blocking operations in C
	using mpi_bsend_t = SymbolDecl<int, const void*, int, MPI_Datatype, int, int, MPI_Comm>;
	using mpi_recv_t = SymbolDecl<int, void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Status*>;
	using mpi_rsend_t = SymbolDecl<int, const void*, int, MPI_Datatype, int, int, MPI_Comm>;
	using mpi_sendrecv_t = SymbolDecl<int, const void*, int, MPI_Datatype, int, int, void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Status*>;
	using mpi_sendrecv_replace_t = SymbolDecl<int, void*, int, MPI_Datatype, int, int, int, int, MPI_Comm, MPI_Status*>;
	using mpi_ssend_t = SymbolDecl<int, const void*, int, MPI_Datatype, int, int, MPI_Comm>;
	using mpi_send_t = SymbolDecl<int, const void*, int, MPI_Datatype, int, int, MPI_Comm>;

	//! Point-to-point non-blocking operations in C
	using mpi_ibsend_t = SymbolDecl<int, const void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*>;
	using mpi_irecv_t = SymbolDecl<int, void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*>;
	using mpi_irsend_t = SymbolDecl<int, const void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*>;
	using mpi_issend_t = SymbolDecl<int, const void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*>;
	using mpi_isend_t = SymbolDecl<int, const void*, int, MPI_Datatype, int, int, MPI_Comm, MPI_Request*>;

	//! Collective blocking operations in C
	using mpi_allgather_t = SymbolDecl<int, const void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm>;
	using mpi_allgatherv_t = SymbolDecl<int, const void*, int, MPI_Datatype, void*, const int[], const int[], MPI_Datatype, MPI_Comm>;
	using mpi_allreduce_t = SymbolDecl<int, const void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm>;
	using mpi_alltoall_t = SymbolDecl<int, const void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm>;
	using mpi_alltoallv_t = SymbolDecl<int, const void*, const int[], const int[], MPI_Datatype, void*, const int[], const int[], MPI_Datatype, MPI_Comm>;
	using mpi_alltoallw_t = SymbolDecl<int, const void*, const int[], const int[], const MPI_Datatype[], void*, const int[], const int[], const MPI_Datatype[], MPI_Comm>;
	using mpi_barrier_t = SymbolDecl<int, MPI_Comm>;
	using mpi_bcast_t = SymbolDecl<int, void*, int, MPI_Datatype, int, MPI_Comm>;
	using mpi_gather_t = SymbolDecl<int, const void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm>;
	using mpi_gatherv_t = SymbolDecl<int, const void*, int, MPI_Datatype, void*, const int[], const int[], MPI_Datatype, int, MPI_Comm>;
	using mpi_reduce_t = SymbolDecl<int, const void*, void*, int, MPI_Datatype, MPI_Op, int, MPI_Comm>;
	using mpi_reduce_scatter_t = SymbolDecl<int, const void*, void*, const int[], MPI_Datatype, MPI_Op, MPI_Comm>;
	using mpi_reduce_scatter_block_t = SymbolDecl<int, const void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm>;
	using mpi_scatter_t = SymbolDecl<int, const void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm>;
	using mpi_scatterv_t = SymbolDecl<int, const void*, const int[], const int[], MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm>;
	using mpi_scan_t = SymbolDecl<int, const void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm>;
	using mpi_exscan_t = SymbolDecl<int, const void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm>;

	//! Collective non-blocking operations in C
	using mpi_iallgather_t = SymbolDecl<int, const void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm, MPI_Request*>;
	using mpi_iallgatherv_t = SymbolDecl<int, const void*, int, MPI_Datatype, void*, const int[], const int[], MPI_Datatype, MPI_Comm, MPI_Request*>;
	using mpi_iallreduce_t = SymbolDecl<int, const void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm, MPI_Request*>;
	using mpi_ialltoall_t = SymbolDecl<int, const void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm, MPI_Request*>;
	using mpi_ialltoallv_t = SymbolDecl<int, const void*, const int[], const int[], MPI_Datatype, void*, const int[], const int[], MPI_Datatype, MPI_Comm, MPI_Request*>;
	using mpi_ialltoallw_t = SymbolDecl<int, const void*, const int[], const int[], const MPI_Datatype[], void*, const int[], const int[], const MPI_Datatype[], MPI_Comm, MPI_Request*>;
	using mpi_ibarrier_t = SymbolDecl<int, MPI_Comm, MPI_Request*>;
	using mpi_ibcast_t = SymbolDecl<int, void*, int, MPI_Datatype, int, MPI_Comm, MPI_Request*>;
	using mpi_igather_t = SymbolDecl<int, const void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm, MPI_Request*>;
	using mpi_igatherv_t = SymbolDecl<int, const void*, int, MPI_Datatype, void*, const int[], const int[], MPI_Datatype, int, MPI_Comm, MPI_Request*>;
	using mpi_ireduce_t = SymbolDecl<int, const void*, void*, int, MPI_Datatype, MPI_Op, int, MPI_Comm, MPI_Request*>;
	using mpi_ireduce_scatter_t = SymbolDecl<int, const void*, void*, const int[], MPI_Datatype, MPI_Op, MPI_Comm, MPI_Request*>;
	using mpi_ireduce_scatter_block_t = SymbolDecl<int, const void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm, MPI_Request*>;
	using mpi_iscatter_t = SymbolDecl<int, const void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm, MPI_Request*>;
	using mpi_iscatterv_t = SymbolDecl<int, const void*, const int[], const int[], MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm, MPI_Request*>;
	using mpi_iscan_t = SymbolDecl<int, const void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm, MPI_Request*>;
	using mpi_iexscan_t = SymbolDecl<int, const void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm, MPI_Request*>;
};


template <>
struct Prototypes<Fortran> {
	//! Setup operations in Fortran
	using mpi_finalize_t = SymbolDecl<void, MPI_Fint*>;
	using mpi_init_t = SymbolDecl<void, MPI_Fint*>;
	using mpi_init_thread_t = SymbolDecl<void, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
	using mpi_comm_rank_t = SymbolDecl<void, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
	using mpi_comm_size_t = SymbolDecl<void, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
	using mpi_query_thread_t = SymbolDecl<void, MPI_Fint*, MPI_Fint*>;

	//! Request testing/waiting operations in Fortran
	using mpi_test_t = SymbolDecl<void, MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
	using mpi_testall_t = SymbolDecl<void, MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[], MPI_Fint*>;
	using mpi_testany_t = SymbolDecl<void, MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint*>;
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
};

template <typename Lang>
struct Names {};

template <>
struct Names<C> {
	//! Request testing operations
	static constexpr std::string_view mpi_test = "MPI_Test";
	static constexpr std::string_view mpi_testall = "MPI_Testall";
	static constexpr std::string_view mpi_testany = "MPI_Testany";
	static constexpr std::string_view mpi_testsome = "MPI_Testsome";

	//! Point-to-point non-blocking operations
	static constexpr std::string_view mpi_ibsend = "MPI_Ibsend";
	static constexpr std::string_view mpi_irecv = "MPI_Irecv";
	static constexpr std::string_view mpi_irsend = "MPI_Irsend";
	static constexpr std::string_view mpi_issend = "MPI_Issend";
	static constexpr std::string_view mpi_isend = "MPI_Isend";

	//! Collective non-blocking operations
	static constexpr std::string_view mpi_iallgather = "MPI_Iallgather";
	static constexpr std::string_view mpi_iallgatherv = "MPI_Iallgatherv";
	static constexpr std::string_view mpi_iallreduce = "MPI_Iallreduce";
	static constexpr std::string_view mpi_ialltoall = "MPI_Ialltoall";
	static constexpr std::string_view mpi_ialltoallv = "MPI_Ialltoallv";
	static constexpr std::string_view mpi_ialltoallw = "MPI_Ialltoallw";
	static constexpr std::string_view mpi_ibarrier = "MPI_Ibarrier";
	static constexpr std::string_view mpi_ibcast = "MPI_Ibcast";
	static constexpr std::string_view mpi_igather = "MPI_Igather";
	static constexpr std::string_view mpi_igatherv = "MPI_Igatherv";
	static constexpr std::string_view mpi_ireduce = "MPI_Ireduce";
	static constexpr std::string_view mpi_ireduce_scatter = "MPI_Ireduce_scatter";
	static constexpr std::string_view mpi_ireduce_scatter_block = "MPI_Ireduce_scatter_block";
	static constexpr std::string_view mpi_iscatter = "MPI_Iscatter";
	static constexpr std::string_view mpi_iscatterv = "MPI_Iscatterv";
	static constexpr std::string_view mpi_iscan = "MPI_Iscan";
	static constexpr std::string_view mpi_iexscan = "MPI_Iexscan";

	//! Other operations
	static constexpr std::string_view mpi_comm_rank = "MPI_Comm_rank";
	static constexpr std::string_view mpi_comm_size = "MPI_Comm_size";
	static constexpr std::string_view mpi_query_thread = "MPI_Query_thread";
};

template <>
struct Names<Fortran> {
	//! Request testing operations
	static constexpr std::string_view mpi_test = "mpi_test_";
	static constexpr std::string_view mpi_testall = "mpi_testall_";
	static constexpr std::string_view mpi_testany = "mpi_testany_";
	static constexpr std::string_view mpi_testsome = "mpi_testsome_";

	//! Point-to-point non-blocking operations
	static constexpr std::string_view mpi_ibsend = "mpi_ibsend_";
	static constexpr std::string_view mpi_irecv = "mpi_irecv_";
	static constexpr std::string_view mpi_irsend = "mpi_irsend_";
	static constexpr std::string_view mpi_issend = "mpi_issend_";
	static constexpr std::string_view mpi_isend = "mpi_isend_";

	//! Collective non-blocking operations
	static constexpr std::string_view mpi_iallgather = "mpi_iallgather_";
	static constexpr std::string_view mpi_iallgatherv = "mpi_iallgatherv_";
	static constexpr std::string_view mpi_iallreduce = "mpi_iallreduce_";
	static constexpr std::string_view mpi_ialltoall = "mpi_ialltoall_";
	static constexpr std::string_view mpi_ialltoallv = "mpi_ialltoallv_";
	static constexpr std::string_view mpi_ialltoallw = "mpi_ialltoallw_";
	static constexpr std::string_view mpi_ibarrier = "mpi_ibarrier_";
	static constexpr std::string_view mpi_ibcast = "mpi_ibcast_";
	static constexpr std::string_view mpi_igather = "mpi_igather_";
	static constexpr std::string_view mpi_igatherv = "mpi_igatherv_";
	static constexpr std::string_view mpi_ireduce = "mpi_ireduce_";
	static constexpr std::string_view mpi_ireduce_scatter = "mpi_ireduce_scatter_";
	static constexpr std::string_view mpi_ireduce_scatter_block = "mpi_ireduce_scatter_block_";
	static constexpr std::string_view mpi_iscatter = "mpi_iscatter_";
	static constexpr std::string_view mpi_iscatterv = "mpi_iscatterv_";
	static constexpr std::string_view mpi_iscan = "mpi_iscan_";
	static constexpr std::string_view mpi_iexscan = "mpi_iexscan_";

	//! Other operations
	static constexpr std::string_view mpi_comm_rank = "mpi_comm_rank_";
	static constexpr std::string_view mpi_comm_size = "mpi_comm_size_";
	static constexpr std::string_view mpi_query_thread = "mpi_query_thread_";
};

} // namespace tampi

#endif // DECLARATIONS_HPP
