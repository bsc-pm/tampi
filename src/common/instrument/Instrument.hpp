/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2021-2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef INSTRUMENT_HPP
#define INSTRUMENT_HPP

#include "util/EnvironmentVariable.hpp"

namespace tampi {

//! The instrumentation points
enum InstrumentPoint {
	AddQueues = 0,
	CheckGlobalRequestArray,
	CompletedRequest,
	CreateTicket,
	IssueNonBlockingOp,
	LibraryInterface,
	LibraryPolling,
	TestAllRequests,
	TestRequest,
	TestSomeRequests,
	TransferQueues,
	WaitTicket,
	NumInstrumentPoints,
};

//! The instrumentation backend interface
class InstrumentBackendInterface {
public:
	virtual void initialize(int rank, int nranks) = 0;
	virtual void finalize() = 0;
	virtual void enter(InstrumentPoint point) = 0;
	virtual void exit(InstrumentPoint point) = 0;
};

class Instrument {
private:
	//! Envar controlling whether the instrumentation should be enabled. The
	//! envar is called TAMPI_INSTRUMENT and the default value is "none". The
	//! accepted values are:
	//!   - "none": No instrumentation is enabled
	//!   - "ovni": Ovni instrumentation is enabled to generate Paraver traces
	static EnvironmentVariable<std::string> _instrument;

	//! Reference to the instrumentation backend; null if no instrumentation
	static InstrumentBackendInterface *_backend;

public:
	//! \brief Initialize the instrumentation
	static void initialize(int rank, int nranks);

	//! \brief Finalize the instrumentation
	static inline void finalize()
	{
		if (_backend)
			_backend->finalize();
	}

	//! \brief Enter into a state at an instrument point
	template <InstrumentPoint Point>
	static inline void enter()
	{
		if (_backend)
			_backend->enter(Point);
	}

	//! \brief Exit from a state at an instrument point
	template <InstrumentPoint Point>
	static inline void exit()
	{
		if (_backend)
			_backend->exit(Point);
	}

	//! \brief Guard class to perform automatic scope instrumentation
	//!
	//! Guard objects instrument the enter and exit points of a specific
	//! instrumentation point at construction and destruction, respectively
	template <InstrumentPoint Point>
	struct Guard {
		//! \brief Enter the instrumentation point at construction
		inline Guard()
		{
			Instrument::enter<Point>();
		}

		//! \brief Exit the instrumentation point at destruction
		inline ~Guard()
		{
			Instrument::exit<Point>();
		}
	};
};

} // namespace tampi

#endif // INSTRUMENT_HPP
