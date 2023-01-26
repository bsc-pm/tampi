/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2021-2022 Barcelona Supercomputing Center (BSC)
*/

#ifndef INSTRUMENT_HPP
#define INSTRUMENT_HPP

#include <cmath>
#include <cstddef>
#include <cstdint>

#include <config.h>

#include "util/EnvironmentVariable.hpp"

namespace tampi {

class InstrumentBackend {
public:
	virtual void initialize(int rank, int nranks) = 0;
	virtual void finalize() = 0;
};

class Instrument {
private:
	//! Envar controlling whether the instrumentation should be enabled. The
	//! envar is called TAMPI_INSTRUMENT and the default value is "none". The
	//! accepted values are:
	//!   - "none": No instrumentation is enabled
	//!   - "ovni": Ovni instrumentation is enabled to generate Paraver traces
	static EnvironmentVariable<std::string> _instrument;

	//! Reference to the instrumentation backend. May be null if no instrumentation
	static InstrumentBackend *_backend;

public:
	//! \brief Initialize the instrumentation
	static void initialize(int rank, int nranks);

	//! \brief Finalize the instrumentation
	static inline void finalize()
	{
		if (_backend)
			_backend->finalize();
	}
};

} // namespace tampi

#endif // INSTRUMENT_HPP
