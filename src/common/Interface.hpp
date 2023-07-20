/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2022-2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <mpi.h>
#include <array>

#include "Symbol.hpp"

namespace tampi {

//! Classes representing C/C++ and Fortran languages. The MPI interface
//! is very different between the C/C++ and the Fortran launagues, so we
//! try to define common code and we use specific classes with the
//! differences, such as MPI functions and types.
class C {};
class Fortran {};

//! Class for defining typedefs
template <typename Lang>
struct Types {};

template <>
struct Types<C> {
	typedef MPI_Request request_t;
	typedef MPI_Status status_t;
	typedef MPI_Status* status_ptr_t;
};

template <>
struct Types<Fortran> {
	typedef MPI_Fint request_t;
	typedef std::array<MPI_Fint, sizeof(MPI_Status)/sizeof(MPI_Fint)> status_t;
	typedef MPI_Fint* status_ptr_t;
};

//! Class providing MPI constants and functions. Each function must be specialized
//! for both C/C++ and Fortran languages
template <typename Lang>
class Interface {
	typedef typename Types<Lang>::request_t request_t;
	typedef typename Types<Lang>::status_t status_t;
	typedef typename Types<Lang>::status_ptr_t status_ptr_t;

public:
	static request_t REQUEST_NULL;
	static status_ptr_t STATUS_IGNORE;
	static status_ptr_t STATUSES_IGNORE;
	static int rank;
	static int nranks;

	static void initialize();

	static int test(request_t &request, int &finished, status_ptr_t status);
	static int testall(int size, request_t *requests, int &finished, status_ptr_t statuses);
	static int testsome(int size, request_t *requests, int &completed, int *indices, status_ptr_t statuses);
};

template <>
inline void Interface<C>::initialize()
{
	REQUEST_NULL = MPI_REQUEST_NULL;
	STATUS_IGNORE = MPI_STATUS_IGNORE;
	STATUSES_IGNORE = MPI_STATUS_IGNORE;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nranks);
}

template <>
inline void Interface<Fortran>::initialize()
{
	REQUEST_NULL = MPI_Request_c2f(MPI_REQUEST_NULL);
	STATUS_IGNORE = MPI_F_STATUS_IGNORE;
	STATUSES_IGNORE = MPI_F_STATUSES_IGNORE;

	MPI_Fint comm = MPI_Comm_c2f(MPI_COMM_WORLD);

	int err;
	mpi_comm_rank_(&comm, &rank, &err);
	mpi_comm_size_(&comm, &nranks, &err);
}

template <>
inline int Interface<C>::test(request_t &request, int &finished, status_ptr_t status)
{
	return PMPI_Test(&request, &finished, status);
}

template <>
inline int Interface<C>::testall(int size, request_t *requests, int &finished, status_ptr_t statuses)
{
	return PMPI_Testall(size, requests, &finished, statuses);
}

template <>
inline int Interface<C>::testsome(int size, request_t *requests, int &completed, int *indices, status_ptr_t statuses)
{
	return PMPI_Testsome(size, requests, &completed, indices, statuses);
}

template <>
inline int Interface<Fortran>::test(request_t &request, int &finished, status_ptr_t status)
{
	int err;
	pmpi_test_(&request, &finished, status, &err);
	return err;
}

template <>
inline int Interface<Fortran>::testall(int size, request_t *requests, int &finished, status_ptr_t statuses)
{
	int err;
	pmpi_testall_(&size, requests, &finished, statuses, &err);
	return err;
}

template <>
inline int Interface<Fortran>::testsome(int size, request_t *requests, int &completed, int *indices, status_ptr_t statuses)
{
	int err;
	pmpi_testsome_(&size, requests, &completed, indices, statuses, &err);
	if (err != MPI_SUCCESS || completed == MPI_UNDEFINED)
		return err;

	// Decrease the indices by one to match C/C++ interface beahvior
	for (int r = 0; r < completed; ++r)
		indices[r] = indices[r] - 1;

	return MPI_SUCCESS;
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

#if !defined(DISABLE_C_LANG)
using InterfaceAny = Interface<C>;
#elif !defined(DISABLE_FORTRAN_LANG)
using InterfaceAny = Interface<Fortran>;
#endif

} // namespace tampi

#endif // INTERFACE_HPP
