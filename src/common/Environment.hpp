/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2022 Barcelona Supercomputing Center (BSC)
*/

#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <mpi.h>

#include <atomic>
#include <cstdint>

#include "Interface.hpp"
#include "Polling.hpp"
#include "TaskContext.hpp"
#include "TaskingModel.hpp"
#include "TicketManager.hpp"
#include "instrument/Instrument.hpp"
#include "util/ErrorHandler.hpp"


namespace tampi {

//! Class that represents the environment and controls
//! which TAMPI modes are enabled
class Environment {
private:
	//! Indicate whether the blocking TAMPI mode is enabled
	static std::atomic<bool> _blockingEnabled;

	//! Indicate whether the non-blocking TAMPI mode is enabled
	static std::atomic<bool> _nonBlockingEnabled;

public:
	Environment() = delete;
	Environment(const Environment &) = delete;
	const Environment& operator= (const Environment &) = delete;

	//! \brief Check whether the blocking TAMPI mode is enabled
	//!
	//! \returns Whether the blocking mode is enabled
	static inline bool isBlockingEnabled()
	{
		return _blockingEnabled.load();
	}

	//! \brief Check whether the non-blocking TAMPI mode is enabled
	//!
	//! \returns Whether the non-blocking mode is enabled
	static inline bool isNonBlockingEnabled()
	{
		return _nonBlockingEnabled.load();
	}

	//! \brief Initialize the environment of TAMPI
	//!
	//! This function should be called after a successful call to
	//! the original MPI_Init or MPI_Init_thread functions
	//!
	//! \param blockingMode Whether the blocking mode is enabled
	//! \param nonBlockingMode Whether the non-blocking mode is enabled
	static void initialize(bool blockingMode, bool nonBlockingMode)
	{
		assert(!_blockingEnabled);
		assert(!_nonBlockingEnabled);

		_blockingEnabled = blockingMode;
		_nonBlockingEnabled = nonBlockingMode;

#if !defined(DISABLE_C_LANG)
		Interface<C>::initialize();
#endif

#if !defined(DISABLE_FORTRAN_LANG)
		Interface<Fortran>::initialize();
#endif

#if !defined(DISABLE_C_LANG)
		int rank = Interface<C>::rank;
		int nranks = Interface<C>::nranks;
#elif !defined(DISABLE_FORTRAN_LANG)
		int rank = Interface<Fortran>::rank;
		int nranks = Interface<Fortran>::nranks;
#endif

		TaskingModel::initialize(blockingMode, nonBlockingMode);

		Instrument::initialize(rank, nranks);

		if (blockingMode || nonBlockingMode)
			Polling::initialize();
	}

	//! \brief Finalize the environment of TAMPI
	//!
	//! This function should be called after a successful call to
	//! the original MPI_Finalize function. After that call, the
	//! TAMPI library cannot call any MPI function
	static void finalize()
	{
		Instrument::finalize();

		if (isBlockingEnabled() || isNonBlockingEnabled()) {
			Polling::finalize();

			_blockingEnabled = false;
			_nonBlockingEnabled = false;
		}
	}
};

} // namespace tampi

#endif // ENVIRONMENT_HPP
