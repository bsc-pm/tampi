/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2022 Barcelona Supercomputing Center (BSC)
*/

#include <cstdio>

#include "Instrument.hpp"
#include "util/ErrorHandler.hpp"

#ifdef HAVE_OVNI
#include "OvniInstrument.hpp"
#endif

namespace tampi {

EnvironmentVariable<std::string> Instrument::_instrument("TAMPI_INSTRUMENT", "none");
InstrumentBackend *Instrument::_backend = nullptr;

void Instrument::initialize(int rank, int nranks)
{
	if (_instrument.get() == "ovni") {
#ifdef HAVE_OVNI
		_backend = new OvniInstrumentBackend();
#else
		ErrorHandler::fail("TAMPI was not configured with ovni support");
#endif
	} else if (_instrument.get() != "none") {
		ErrorHandler::fail(_instrument.get(), " instrumentation not supported");
	}

	if (_backend)
		_backend->initialize(rank, nranks);
}

} // namespace tampi
