/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include "environment.h"

template<>
bool C::Environment::_enabled = false;

template<>
bool Fortran::Environment::_enabled = false;
