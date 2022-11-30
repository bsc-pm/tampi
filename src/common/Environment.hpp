/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2022 Barcelona Supercomputing Center (BSC)
*/

#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <mpi.h>

#include <atomic>

#include "TaskContext.hpp"
#include "TaskingModel.hpp"
#include "TicketManager.hpp"
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

	//! The handle to the polling instance that periodically checks
	//! the completion of the in-flight MPI requests in TAMPI
	static TaskingModel::polling_handle_t _pollingHandle;

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

		uint64_t pollingPeriod = getPollingPeriod();

		if (blockingMode || nonBlockingMode) {
			_pollingHandle = TaskingModel::registerPolling("TAMPI", Environment::polling, nullptr, pollingPeriod);
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

	//! \brief Get the polling period for TAMPI services
	//!
	//! Determines the polling period in which TAMPI requests will be
	//! checked. Notice this polling period is ignored when running with
	//! the obsolete polling services API
	//!
	//! \returns The polling period in microseconds
	static uint64_t getPollingPeriod()
	{
		EnvironmentVariable<uint64_t> pollingFrequency("TAMPI_POLLING_FREQUENCY");
		EnvironmentVariable<uint64_t> pollingPeriod("TAMPI_POLLING_PERIOD");

		if (pollingFrequency.isPresent()) {
			ErrorHandler::warn("TAMPI_POLLING_FREQUENCY is deprecated; use TAMPI_POLLING_PERIOD instead");
		}

		uint64_t period = 100;
		if (pollingPeriod.isPresent()) {
			period = pollingPeriod.get();
		} else if (pollingFrequency.isPresent()) {
			period = pollingFrequency.get();
		}

		return period;
	}
};

} // namespace tampi

#endif // ENVIRONMENT_HPP
