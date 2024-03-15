/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef POLLING_HPP
#define POLLING_HPP

#include <cstdint>

#include "PollingPeriodCtrl.hpp"
#include "TaskingModel.hpp"
#include "TicketManager.hpp"
#include "instrument/Instrument.hpp"

namespace tampi {

//! Class that represents the polling features
class Polling {
private:
	//! The polling instance of the task that periodically checks the in-flight
	//! MPI requests
	static TaskingModel::PollingInstance *_pollingInstance;

	//! The controller of the polling period
	static PollingPeriodCtrl _periodCtrl;

	//! The polling instance of the task that processes completions
	static TaskingModel::PollingInstance *_completionPollingInstance;

public:
	Polling() = delete;
	Polling(const Polling &) = delete;
	const Polling& operator= (const Polling &) = delete;

	//! \brief Initialize the polling features
	static void initialize()
	{
		_pollingInstance = TaskingModel::registerPolling("TAMPI", Polling::polling, nullptr);

		if (CompletionManager::isEnabled())
			_completionPollingInstance = TaskingModel::registerPolling(
					"TAMPI Comp", Polling::completions, nullptr);
	}

	//! \brief Finalize the polling features
	static void finalize()
	{
		TaskingModel::unregisterPolling(_pollingInstance);

		if (CompletionManager::isEnabled())
			TaskingModel::unregisterPolling(_completionPollingInstance);
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
		size_t completed = 0;

		Instrument::Guard<LibraryPolling> instrGuard;

#ifndef DISABLE_C_LANG
		TicketManager<C> &cManager = TicketManager<C>::get();
		completed += cManager.checkRequests(pending);
#endif
#ifndef DISABLE_FORTRAN_LANG
		TicketManager<Fortran> &fortranManager = TicketManager<Fortran>::get();
		completed += fortranManager.checkRequests(pending);
#endif
		return _periodCtrl.getPeriod(completed, pending);
	}

	static uint64_t completions(void *, uint64_t)
	{
		CompletionManager::process();

		return CompletionManager::getPeriod();
	}
};

} // namespace tampi

#endif // POLLING_HPP
