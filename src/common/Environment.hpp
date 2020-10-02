/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2020 Barcelona Supercomputing Center (BSC)
*/

#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <mpi.h>

#include <atomic>

#include "TaskContext.hpp"
#include "TaskingModel.hpp"
#include "TicketManager.hpp"


namespace tampi {

//! Class that represents the environment and controls
//! which TAMPI modes are enabled
class Environment {
private:
	//! Indicate whether the blocking TAMPI mode is enabled
	static std::atomic<bool> _blockingEnabled;

	//! Indicate whether the non-blocking TAMPI mode is enabled
	static std::atomic<bool> _nonBlockingEnabled;

	//! The handle to the polling instance that periodically checks
	//! the completion of the in-flight MPI requests in TAMPI
	static TaskingModel::polling_handle_t _pollingHandle;

	//! Determine the polling frequency when the TAMPI polling is
	//! implemented with tasks that are paused periodically. That is
	//! the frequency in time (microseconds) at which the in-flight
	//! MPI requests are checked in TAMPI. This environment variable
	//! is called TAMPI_POLLING_FREQUENCY and the default value is
	//! 500 microseconds. Note that this frequency is ignored when
	//! using polling services; instead the environment variable
	//! NANOS6_POLLING_FREQUENCY should be used for OmpSs-2
	static EnvironmentVariable<uint64_t> _pollingFrequency;

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
		assert(!_pollingHandle);

		_blockingEnabled = blockingMode;
		_nonBlockingEnabled = nonBlockingMode;

#ifndef DISABLE_C_LANG
		C::initialize();
#endif
#ifndef DISABLE_FORTRAN_LANG
		Fortran::initialize();
#endif

		TaskingModel::initialize(blockingMode, nonBlockingMode);

		if (blockingMode || nonBlockingMode) {
			_pollingHandle = TaskingModel::registerPolling("TAMPI", Environment::polling, nullptr, _pollingFrequency);
		}
	}

	//! \brief Finalize the environment of TAMPI
	//!
	//! This function should be called after a successful call to
	//! the original MPI_Finalize function. After that call, the
	//! TAMPI library cannot call any MPI function
	static void finalize()
	{
		if (isBlockingEnabled() || isNonBlockingEnabled()) {
			TaskingModel::unregisterPolling(_pollingHandle);

			_blockingEnabled = false;
			_nonBlockingEnabled = false;
		}
	}

	//! \brief Get the ticket manager
	//!
	//! This function requires a template parameter representing
	//! the programming language, which can be C or Fortran for
	//! the moment
	//!
	//! \returns A reference to the ticket manager
	template <typename Lang>
	static inline TicketManager<Lang> &getTicketManager()
	{
		static TicketManager<Lang> _ticketManager;
		return _ticketManager;
	}

private:
	//! \brief Polling function that checks the in-flight requests
	//!
	//! This function is periodically called by the tasking runtime
	//! system and should check for the in-flight MPI requests posted
	//! from both C and Fortran languages (if needed)
	//!
	//! \param args An opaque pointer to the arguments
	static void polling(void *)
	{
#ifndef DISABLE_C_LANG
		TicketManager<C> &cManager = getTicketManager<C>();
		cManager.checkRequests();
#endif
#ifndef DISABLE_FORTRAN_LANG
		TicketManager<Fortran> &fortranManager = getTicketManager<Fortran>();
		fortranManager.checkRequests();
#endif
	}
};

} // namespace tampi

#endif // ENVIRONMENT_HPP
