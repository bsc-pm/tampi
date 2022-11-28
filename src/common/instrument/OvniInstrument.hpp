/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2022 Barcelona Supercomputing Center (BSC)
*/

#ifndef OVNI_INSTRUMENT_HPP
#define OVNI_INSTRUMENT_HPP

#include <cmath>
#include <cstddef>
#include <cstdint>

#include <config.h>

#include <ovni.h>


namespace tampi {

class OvniInstrumentBackend : public InstrumentBackend {
public:
	//! \brief Initialize the ovni instrumentation
	inline void initialize(int rank, int nranks) override
	{
		ovni_proc_set_rank(rank, nranks);
	}

	//! \brief Finalize the ovni instrumentation
	inline void finalize() override
	{
	}
};

} // namespace tampi

#endif // OVNI_INSTRUMENT_HPP
