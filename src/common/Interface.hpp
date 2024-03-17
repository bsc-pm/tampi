/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2022-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <mpi.h>
#include <array>
#include <string_view>

#include "Declarations.hpp"
#include "Symbol.hpp"
#include "util/ErrorHandler.hpp"

namespace tampi {

//! Class for defining typedefs
template <typename Lang>
struct Types {};

template <>
struct Types<C> {
	typedef MPI_Request request_t;
	typedef MPI_Status status_t;
	typedef MPI_Status* status_ptr_t;
	typedef int int_t;
	typedef MPI_Datatype datatype_t;
	typedef MPI_Op op_t;
	typedef MPI_Comm comm_t;
};

template <>
struct Types<Fortran> {
	typedef MPI_Fint request_t;
	typedef std::array<MPI_Fint, sizeof(MPI_Status)/sizeof(MPI_Fint)> status_t;
	typedef MPI_Fint* status_ptr_t;
	typedef MPI_Fint int_t;
	typedef MPI_Fint* datatype_t;
	typedef MPI_Fint op_t;
	typedef MPI_Fint comm_t;
};

//! Class providing MPI constants and functions. Each function must be specialized
//! for both C/C++ and Fortran languages
template <typename Lang>
class Interface {
	typedef typename Types<Lang>::request_t request_t;
	typedef typename Types<Lang>::status_t status_t;
	typedef typename Types<Lang>::status_ptr_t status_ptr_t;

public:
	static Symbol<typename Prototypes<Lang>::mpi_test_t> mpi_test;
	static Symbol<typename Prototypes<Lang>::mpi_testall_t> mpi_testall;
	static Symbol<typename Prototypes<Lang>::mpi_testany_t> mpi_testany;
	static Symbol<typename Prototypes<Lang>::mpi_testsome_t> mpi_testsome;
	static Symbol<typename Prototypes<Lang>::mpi_comm_rank_t> mpi_comm_rank;
	static Symbol<typename Prototypes<Lang>::mpi_comm_size_t> mpi_comm_size;

	static Symbol<typename Prototypes<Lang>::mpi_ibsend_t> mpi_ibsend;
	static Symbol<typename Prototypes<Lang>::mpi_irecv_t> mpi_irecv;
	static Symbol<typename Prototypes<Lang>::mpi_irsend_t> mpi_irsend;
	static Symbol<typename Prototypes<Lang>::mpi_isend_t> mpi_isend;
	static Symbol<typename Prototypes<Lang>::mpi_issend_t> mpi_issend;

	static Symbol<typename Prototypes<Lang>::mpi_iallgather_t> mpi_iallgather;
	static Symbol<typename Prototypes<Lang>::mpi_iallgatherv_t> mpi_iallgatherv;
	static Symbol<typename Prototypes<Lang>::mpi_iallreduce_t> mpi_iallreduce;
	static Symbol<typename Prototypes<Lang>::mpi_ialltoall_t>  mpi_ialltoall;
	static Symbol<typename Prototypes<Lang>::mpi_ialltoallv_t> mpi_ialltoallv;
	static Symbol<typename Prototypes<Lang>::mpi_ialltoallw_t> mpi_ialltoallw;
	static Symbol<typename Prototypes<Lang>::mpi_ibarrier_t> mpi_ibarrier;
	static Symbol<typename Prototypes<Lang>::mpi_ibcast_t> mpi_ibcast;
	static Symbol<typename Prototypes<Lang>::mpi_igather_t> mpi_igather;
	static Symbol<typename Prototypes<Lang>::mpi_igatherv_t> mpi_igatherv;
	static Symbol<typename Prototypes<Lang>::mpi_ireduce_t> mpi_ireduce;
	static Symbol<typename Prototypes<Lang>::mpi_ireduce_scatter_t> mpi_ireduce_scatter;
	static Symbol<typename Prototypes<Lang>::mpi_ireduce_scatter_block_t> mpi_ireduce_scatter_block;
	static Symbol<typename Prototypes<Lang>::mpi_iscatter_t> mpi_iscatter;
	static Symbol<typename Prototypes<Lang>::mpi_iscatterv_t> mpi_iscatterv;
	static Symbol<typename Prototypes<Lang>::mpi_iscan_t> mpi_iscan;
	static Symbol<typename Prototypes<Lang>::mpi_iexscan_t> mpi_iexscan;

	static request_t REQUEST_NULL;
	static status_ptr_t STATUS_IGNORE;
	static status_ptr_t STATUSES_IGNORE;
	static int rank;
	static int nranks;

	static void initialize();
	static void loadSymbols();

	static bool test(request_t &request, status_ptr_t status);
	static bool testall(int size, request_t *requests, status_ptr_t statuses);
	static bool testany(int size, request_t *requests, int *index, status_ptr_t status);
	static int testsome(int size, request_t *requests, int *indices, status_ptr_t statuses);
};

template <>
inline void Interface<C>::initialize()
{
	REQUEST_NULL = MPI_REQUEST_NULL;
	STATUS_IGNORE = MPI_STATUS_IGNORE;
	STATUSES_IGNORE = MPI_STATUS_IGNORE;

	loadSymbols();

	mpi_comm_rank(MPI_COMM_WORLD, &rank);
	mpi_comm_size(MPI_COMM_WORLD, &nranks);
}

template <>
inline void Interface<Fortran>::initialize()
{
	REQUEST_NULL = MPI_Request_c2f(MPI_REQUEST_NULL);
	STATUS_IGNORE = MPI_F_STATUS_IGNORE;
	STATUSES_IGNORE = MPI_F_STATUSES_IGNORE;

	loadSymbols();

	MPI_Fint comm = MPI_Comm_c2f(MPI_COMM_WORLD);
	MPI_Fint err;
	mpi_comm_rank(&comm, &rank, &err);
	mpi_comm_size(&comm, &nranks, &err);
}

template <typename Lang>
inline void Interface<Lang>::loadSymbols()
{
	mpi_test.load(SymbolAttr::Next, true);
	mpi_testall.load(SymbolAttr::Next, true);
	mpi_testany.load(SymbolAttr::Next, true);
	mpi_testsome.load(SymbolAttr::Next, true);
	mpi_comm_rank.load(SymbolAttr::Next, true);
	mpi_comm_size.load(SymbolAttr::Next, true);

	mpi_ibsend.load(SymbolAttr::Next, true);
	mpi_irecv.load(SymbolAttr::Next, true);
	mpi_irsend.load(SymbolAttr::Next, true);
	mpi_isend.load(SymbolAttr::Next, true);
	mpi_issend.load(SymbolAttr::Next, true);

	mpi_iallgather.load(SymbolAttr::Next, true);
	mpi_iallgatherv.load(SymbolAttr::Next, true);
	mpi_iallreduce.load(SymbolAttr::Next, true);
	mpi_ialltoall.load(SymbolAttr::Next, true);
	mpi_ialltoallv.load(SymbolAttr::Next, true);
	mpi_ialltoallw.load(SymbolAttr::Next, true);
	mpi_ibarrier.load(SymbolAttr::Next, true);
	mpi_ibcast.load(SymbolAttr::Next, true);
	mpi_igather.load(SymbolAttr::Next, true);
	mpi_igatherv.load(SymbolAttr::Next, true);
	mpi_ireduce.load(SymbolAttr::Next, true);
	mpi_ireduce_scatter.load(SymbolAttr::Next, true);
	mpi_ireduce_scatter_block.load(SymbolAttr::Next, true);
	mpi_iscatter.load(SymbolAttr::Next, true);
	mpi_iscatterv.load(SymbolAttr::Next, true);
	mpi_iscan.load(SymbolAttr::Next, true);
	mpi_iexscan.load(SymbolAttr::Next, true);
}

template <>
inline bool Interface<C>::test(request_t &request, status_ptr_t status)
{
	int completed;
	int err = mpi_test(&request, &completed, status);
	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Unexpected return code from MPI_Test");

	return completed;
}

template <>
inline bool Interface<C>::testall(int size, request_t *requests, status_ptr_t statuses)
{
	int completed;
	int err = mpi_testall(size, requests, &completed, statuses);
	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Unexpected return code from MPI_Testall");

	return completed;
}

template <>
inline bool Interface<C>::testany(int size, request_t *requests, int *index, status_ptr_t status)
{
	int completed;
	int err = mpi_testany(size, requests, index, &completed, status);
	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Unexpected return code from MPI_Testany");
	else if (completed && *index == MPI_UNDEFINED)
		// Abort the program since this case should never occur.
		// The completed MPI request's index is MPI_UNDEFINED
		// when all passed requests were already inactive
		ErrorHandler::fail("Unexpected output from MPI_Testany");

	return completed;
}

template <>
inline int Interface<C>::testsome(int size, request_t *requests, int *indices, status_ptr_t statuses)
{
	int completed;
	int err = mpi_testsome(size, requests, &completed, indices, statuses);
	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Unexpected return code from MPI_Testsome");
	if (completed == MPI_UNDEFINED)
		// Abort the program since this case should never occur.
		// The number of completed MPI requests is MPI_UNDEFINED
		// when all passed requests were already inactive
		ErrorHandler::fail("Unexpected output from MPI_Testsome");

	return completed;
}

template <>
inline bool Interface<Fortran>::test(request_t &request, status_ptr_t status)
{
	int completed, err;
	mpi_test(&request, &completed, status, &err);
	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Unexpected return code from MPI_Test");

	return completed;
}

template <>
inline bool Interface<Fortran>::testall(int size, request_t *requests, status_ptr_t statuses)
{
	int completed, err;
	mpi_testall(&size, requests, &completed, statuses, &err);
	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Unexpected return code from MPI_Testall");
	return completed;
}

template <>
inline bool Interface<Fortran>::testany(int size, request_t *requests, int *index, status_ptr_t status)
{
	int completed, err;
	mpi_testany(&size, requests, index, &completed, status, &err);
	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Unexpected return code from MPI_Testany");
	else if (completed && *index == MPI_UNDEFINED)
		// Abort the program since this case should never occur.
		// The completed MPI request's index is MPI_UNDEFINED
		// when all passed requests were already inactive
		ErrorHandler::fail("Unexpected output from MPI_Testany");

	// Decrease the index by one to match C/C++ interface beahvior
	if (completed)
		*index = *index - 1;

	return completed;
}

template <>
inline int Interface<Fortran>::testsome(int size, request_t *requests, int *indices, status_ptr_t statuses)
{
	int err, completed;
	mpi_testsome(&size, requests, &completed, indices, statuses, &err);
	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Unexpected return code from MPI_Testsome");
	else if (completed == MPI_UNDEFINED)
		// Abort the program since this case should never occur.
		// The number of completed MPI requests is MPI_UNDEFINED
		// when all passed requests were already inactive
		ErrorHandler::fail("Unexpected output from MPI_Testsome");

	// Decrease the indices by one to match C/C++ interface beahvior
	for (int r = 0; r < completed; ++r)
		indices[r] = indices[r] - 1;

	return completed;
}

template <typename Lang>
typename Types<Lang>::request_t Interface<Lang>::REQUEST_NULL;
template <typename Lang>
typename Types<Lang>::status_ptr_t Interface<Lang>::STATUS_IGNORE;
template <typename Lang>
typename Types<Lang>::status_ptr_t Interface<Lang>::STATUSES_IGNORE;

template <typename Lang>
int Interface<Lang>::rank;
template <typename Lang>
int Interface<Lang>::nranks;

template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_test_t> Interface<Lang>::mpi_test(Names<Lang>::mpi_test, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_testall_t> Interface<Lang>::mpi_testall(Names<Lang>::mpi_testall, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_testany_t> Interface<Lang>::mpi_testany(Names<Lang>::mpi_testany, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_testsome_t> Interface<Lang>::mpi_testsome(Names<Lang>::mpi_testsome, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_comm_rank_t> Interface<Lang>::mpi_comm_rank(Names<Lang>::mpi_comm_rank, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_comm_size_t> Interface<Lang>::mpi_comm_size(Names<Lang>::mpi_comm_size, false);

template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_ibsend_t> Interface<Lang>::mpi_ibsend(Names<Lang>::mpi_ibsend, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_irecv_t> Interface<Lang>::mpi_irecv(Names<Lang>::mpi_irecv, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_irsend_t> Interface<Lang>::mpi_irsend(Names<Lang>::mpi_irsend, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_isend_t> Interface<Lang>::mpi_isend(Names<Lang>::mpi_isend, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_issend_t> Interface<Lang>::mpi_issend(Names<Lang>::mpi_issend, false);

template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_iallgather_t> Interface<Lang>::mpi_iallgather(Names<Lang>::mpi_iallgather, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_iallgatherv_t> Interface<Lang>::mpi_iallgatherv(Names<Lang>::mpi_iallgatherv, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_iallreduce_t> Interface<Lang>::mpi_iallreduce(Names<Lang>::mpi_iallreduce, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_ialltoall_t>  Interface<Lang>::mpi_ialltoall(Names<Lang>::mpi_ialltoall, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_ialltoallv_t> Interface<Lang>::mpi_ialltoallv(Names<Lang>::mpi_ialltoallv, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_ialltoallw_t> Interface<Lang>::mpi_ialltoallw(Names<Lang>::mpi_ialltoallw, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_ibarrier_t> Interface<Lang>::mpi_ibarrier(Names<Lang>::mpi_ibarrier, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_ibcast_t> Interface<Lang>::mpi_ibcast(Names<Lang>::mpi_ibcast, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_igather_t> Interface<Lang>::mpi_igather(Names<Lang>::mpi_igather, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_igatherv_t> Interface<Lang>::mpi_igatherv(Names<Lang>::mpi_igatherv, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_ireduce_t> Interface<Lang>::mpi_ireduce(Names<Lang>::mpi_ireduce, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_ireduce_scatter_t> Interface<Lang>::mpi_ireduce_scatter(Names<Lang>::mpi_ireduce_scatter, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_ireduce_scatter_block_t> Interface<Lang>::mpi_ireduce_scatter_block(Names<Lang>:: mpi_ireduce_scatter_block, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_iscatter_t> Interface<Lang>::mpi_iscatter(Names<Lang>::mpi_iscatter, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_iscatterv_t> Interface<Lang>::mpi_iscatterv(Names<Lang>::mpi_iscatterv, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_iscan_t> Interface<Lang>::mpi_iscan(Names<Lang>::mpi_iscan, false);
template <typename Lang>
Symbol<typename Prototypes<Lang>::mpi_iexscan_t> Interface<Lang>::mpi_iexscan(Names<Lang>::mpi_iexscan, false);

#if !defined(DISABLE_C_LANG)
using InterfaceAny = Interface<C>;
#elif !defined(DISABLE_FORTRAN_LANG)
using InterfaceAny = Interface<Fortran>;
#endif

} // namespace tampi

#endif // INTERFACE_HPP
