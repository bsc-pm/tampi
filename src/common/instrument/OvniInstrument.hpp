/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2022-2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef OVNI_INSTRUMENT_HPP
#define OVNI_INSTRUMENT_HPP

#include <ovni.h>

#include "Instrument.hpp"

namespace tampi {

class OvniInstrumentBackend : public InstrumentBackendInterface {
private:
	//! The state is composed by the enter and exit ovni MCV, which are three
	//! characters that specify the event model, category and value
	struct StateInfo {
		const char *enterMCV;
		const char *exitMCV;
	};

	//! The table storing subsystem states information with the enter and
	//! exit event model-category-value. The model for TAMPI events is 'T'
	static constexpr StateInfo Subsystems[NumInstrumentPoints] = {
		[AddQueues] =               { "TQa", "TQA" },
		[CheckGlobalRequestArray] = { "TGc", "TGC" },
		[CompletedRequest] =        { "TRc", "TRC" },
		[CreateTicket] =            { "TTc", "TTC" },
		[IssueNonBlockingOp] =      { "TCi", "TCI" },
		[LibraryInterface] =        { "TLi", "TLI" },
		[LibraryPolling] =          { "TLp", "TLP" },
		[TestAllRequests] =         { "TRa", "TRA" },
		[TestRequest] =             { "TRt", "TRT" },
		[TestSomeRequests] =        { "TRs", "TRS" },
		[TransferQueues] =          { "TQt", "TQT" },
		[WaitTicket] =              { "TTw", "TTW" },
	};

	//! Emit an ovni event given the event model-category-value
	static void emitEvent(const char *mcv)
	{
		struct ovni_ev ev = {};
		ovni_ev_set_clock(&ev, ovni_clock_now());
		ovni_ev_set_mcv(&ev, mcv);
		ovni_ev_emit(&ev);
	}

public:
	//! \brief Initialize the ovni instrumentation
	void initialize(int rank, int nranks) override
	{
		ovni_proc_set_rank(rank, nranks);
	}

	//! \brief Finalize the ovni instrumentation
	void finalize() override
	{
	}

	//! \brief Enter into a subsystem state at an instrument point
	void enter(InstrumentPoint point) override
	{
		emitEvent(Subsystems[point].enterMCV);
	}

	//! \brief Exit from a subsystem state at an instrument point
	void exit(InstrumentPoint point) override
	{
		emitEvent(Subsystems[point].exitMCV);
	}
};

} // namespace tampi

#endif // OVNI_INSTRUMENT_HPP
