/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include "definitions.h"
#include "environment.h"

template<>
std::atomic<bool> C::Environment::_enabled(false);

template<>
std::atomic<bool> Fortran::Environment::_enabled(false);

// The actual value must be obtained by calling to the
// MPI_Request_c2f function after the initialization
// of MPI
MPI_Fint MPI_F_REQUEST_NULL = 0;
