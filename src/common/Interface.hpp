/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2022-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <mpi.h>
#include <array>

#include "Declarations.hpp"
#include "Symbol.hpp"
#include "util/ErrorHandler.hpp"

namespace tampi {

//! Classes representing C/C++ and Fortran languages. The MPI interface
//! is very different between the C/C++ and the Fortran launagues, so we
//! try to define common code and we use specific classes with the
//! differences, such as MPI functions and types.
struct C {
	static constexpr const char *Test = "MPI_Test";
	static constexpr const char *Testall = "MPI_Testall";
	static constexpr const char *Testany = "MPI_Testany";
	static constexpr const char *Testsome = "MPI_Testsome";
};

struct Fortran {
	static constexpr const char *Test = "mpi_test_";
	static constexpr const char *Testall = "mpi_testall_";
	static constexpr const char *Testany = "mpi_testany_";
	static constexpr const char *Testsome = "mpi_testsome_";
};

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

	typedef MPI_Test_t test_t;
	typedef MPI_Testall_t testall_t;
	typedef MPI_Testany_t testany_t;
	typedef MPI_Testsome_t testsome_t;
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

	typedef mpi_test_t test_t;
	typedef mpi_testall_t testall_t;
	typedef mpi_testany_t testany_t;
	typedef mpi_testsome_t testsome_t;
};

//! Class providing MPI constants and functions. Each function must be specialized
//! for both C/C++ and Fortran languages
template <typename Lang>
class Interface {
	typedef typename Types<Lang>::request_t request_t;
	typedef typename Types<Lang>::status_t status_t;
	typedef typename Types<Lang>::status_ptr_t status_ptr_t;

	static Symbol<typename Types<Lang>::test_t> _test;
	static Symbol<typename Types<Lang>::testall_t> _testall;
	static Symbol<typename Types<Lang>::testany_t> _testany;
	static Symbol<typename Types<Lang>::testsome_t> _testsome;

public:
	static request_t REQUEST_NULL;
	static status_ptr_t STATUS_IGNORE;
	static status_ptr_t STATUSES_IGNORE;
	static int rank;
	static int nranks;

	static void initialize();

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

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nranks);

	_test.load(SymbolAttr::Next, true);
	_testall.load(SymbolAttr::Next, true);
	_testsome.load(SymbolAttr::Next, true);
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

	_test.load(SymbolAttr::Next, true);
	_testall.load(SymbolAttr::Next, true);
	_testsome.load(SymbolAttr::Next, true);
}

template <>
inline bool Interface<C>::test(request_t &request, status_ptr_t status)
{
	int completed;
	int err = _test(&request, &completed, status);
	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Unexpected return code from MPI_Test");

	return completed;
}

template <>
inline bool Interface<C>::testall(int size, request_t *requests, status_ptr_t statuses)
{
	int completed;
	int err = _testall(size, requests, &completed, statuses);
	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Unexpected return code from MPI_Testall");

	return completed;
}

template <>
inline bool Interface<C>::testany(int size, request_t *requests, int *index, status_ptr_t status)
{
	int completed;
	int err = _testany(size, requests, index, &completed, status);
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
	int err = _testsome(size, requests, &completed, indices, statuses);
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
	_test(&request, &completed, status, &err);
	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Unexpected return code from MPI_Test");

	return completed;
}

template <>
inline bool Interface<Fortran>::testall(int size, request_t *requests, status_ptr_t statuses)
{
	int completed, err;
	_testall(&size, requests, &completed, statuses, &err);
	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Unexpected return code from MPI_Testall");
	return completed;
}

template <>
inline bool Interface<Fortran>::testany(int size, request_t *requests, int *index, status_ptr_t status)
{
	int completed, err;
	_testany(&size, requests, index, &completed, status, &err);
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
	_testsome(&size, requests, &completed, indices, statuses, &err);
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
Symbol<typename Types<Lang>::test_t> Interface<Lang>::_test(Lang::Test, false);

template <typename Lang>
Symbol<typename Types<Lang>::testall_t> Interface<Lang>::_testall(Lang::Testall, false);

template <typename Lang>
Symbol<typename Types<Lang>::testany_t> Interface<Lang>::_testany(Lang::Testany, false);

template <typename Lang>
Symbol<typename Types<Lang>::testsome_t> Interface<Lang>::_testsome(Lang::Testsome, false);

#if !defined(DISABLE_C_LANG)
using InterfaceAny = Interface<C>;
#elif !defined(DISABLE_FORTRAN_LANG)
using InterfaceAny = Interface<Fortran>;
#endif

} // namespace tampi

#endif // INTERFACE_HPP
