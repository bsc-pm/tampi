/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2020 Barcelona Supercomputing Center (BSC)
*/

#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include <mpi.h>

#include <array>

//! Read only buffers must be defined as const in versions MPI 3.0 and later
#if MPI_VERSION >= 3
	#define MPI3CONST const
#else
	#define MPI3CONST
#endif


class C {
public:
	typedef MPI_Request request_t;
	typedef MPI_Status status_t;
	typedef MPI_Status* status_ptr_t;

	static request_t REQUEST_NULL;
	static status_ptr_t STATUS_IGNORE;
	static status_ptr_t STATUSES_IGNORE;

	static inline void initialize()
	{
		REQUEST_NULL = MPI_REQUEST_NULL;
		STATUS_IGNORE = MPI_STATUS_IGNORE;
		STATUSES_IGNORE = MPI_STATUS_IGNORE;
	}
};

class Fortran {
public:
	typedef MPI_Fint request_t;
	typedef std::array<MPI_Fint, sizeof(MPI_Status)/sizeof(MPI_Fint)> status_t;
	typedef MPI_Fint* status_ptr_t;

	static request_t REQUEST_NULL;
	static status_ptr_t STATUS_IGNORE;
	static status_ptr_t STATUSES_IGNORE;

	static inline void initialize()
	{
		REQUEST_NULL = MPI_Request_c2f(MPI_REQUEST_NULL);
		STATUS_IGNORE = MPI_F_STATUS_IGNORE;
		STATUSES_IGNORE = MPI_F_STATUSES_IGNORE;
	}
};

#endif // DEFINITIONS_HPP
