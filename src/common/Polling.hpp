/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef POLLING_HPP
#define POLLING_HPP

#include <mpi.h>

#include <atomic>
#include <cassert>
#include <cstdint>

#include "TaskingModel.hpp"
#include "TicketManager.hpp"
#include "util/ErrorHandler.hpp"
#include "util/EnvironmentVariable.hpp"


namespace tampi {

struct PollingFrequencyCtrl {
	//! The static period defined by TAMPI_POLLING_PERIOD
	uint64_t _period;

	//! \brief Create the frequency controller
	//!
	//! The controller reads the TAMPI_POLLING_PERIOD envar to determine the static
	//! period in which TAMPI will check its MPI requests. If the envar is not defined,
	//! the period is 100us as a default value. Notice that TAMPI_POLLING_FREQUENCY is
	//! a deprecated envar
	inline PollingFrequencyCtrl() : _period(100)
	{
		EnvironmentVariable<uint64_t> pollingFrequency("TAMPI_POLLING_FREQUENCY");
		EnvironmentVariable<uint64_t> pollingPeriod("TAMPI_POLLING_PERIOD");

		if (pollingFrequency.isPresent()) {
			ErrorHandler::warn("TAMPI_POLLING_FREQUENCY is deprecated; use TAMPI_POLLING_PERIOD instead");
		}

		// Give precedence to TAMPI_POLLING_PERIOD
		if (pollingPeriod.isPresent()) {
			_period = pollingPeriod.get();
		} else if (pollingFrequency.isPresent()) {
			_period = pollingFrequency.get();
		}
	}

	//! Retrieve the static period in microseconds
	inline uint64_t getPeriod()
	{
		return _period;
	}
};

//! Class that represents the polling features
class Polling {
private:
	//! The handle to the polling instance that periodically checks
	//! the completion of the in-flight MPI requests in TAMPI
	static TaskingModel::polling_handle_t _pollingHandle;

	static PollingFrequencyCtrl _frequencyCtrl;

public:
	Polling() = delete;
	Polling(const Polling &) = delete;
	const Polling& operator= (const Polling &) = delete;

	//! \brief Initialize the polling features
	static void initialize()
	{
		_pollingHandle = TaskingModel::registerPolling("TAMPI", Polling::polling, nullptr);
	}

	//! \brief Finalize the polling features
	static void finalize()
	{
		TaskingModel::unregisterPolling(_pollingHandle);
	}

private:
	//! \brief Polling function that checks the in-flight requests
	//!
	//! This function is periodically called by the tasking runtime system
	//! and should check for the in-flight MPI requests posted from both C
	//! and Fortran languages (if needed)
	//!
	//! \param args An opaque pointer to the arguments
	//! \param prevWaitUs The actual wait time in microseconds
	//!
	//! \returns How many microseconds should the task wait in the next call
	static uint64_t polling(void *, uint64_t)
	{
		size_t pending = 0;
#ifndef DISABLE_C_LANG
		TicketManager<C> &cManager = TicketManager<C>::getTicketManager();
		cManager.checkRequests(pending);
#endif
#ifndef DISABLE_FORTRAN_LANG
		TicketManager<Fortran> &fortranManager = TicketManager<Fortran>::getTicketManager();
		fortranManager.checkRequests(pending);
#endif
		return _frequencyCtrl.getPeriod();
	}
};

} // namespace tampi

#endif // POLLING_HPP
