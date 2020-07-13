/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2020 Barcelona Supercomputing Center (BSC)
*/

#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <atomic>

#include <mpi.h>

#include "TaskContext.hpp"
#include "TaskingModel.hpp"
#include "TicketManager.hpp"


class Environment {
private:
	static std::atomic<bool> _blockingEnabled;
	static std::atomic<bool> _nonBlockingEnabled;

	static TaskingModel::polling_handle_t _pollingHandle;

public:
	Environment() = delete;
	Environment(const Environment &) = delete;
	const Environment& operator= (const Environment &) = delete;

	static inline bool isBlockingEnabled()
	{
		return _blockingEnabled.load();
	}

	static inline bool isNonBlockingEnabled()
	{
		return _nonBlockingEnabled.load();
	}

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
			_pollingHandle = TaskingModel::registerPolling("TAMPI", Environment::polling);
		}
	}

	static void finalize()
	{
		if (isBlockingEnabled() || isNonBlockingEnabled()) {
			TaskingModel::unregisterPolling(_pollingHandle);

			_blockingEnabled = false;
			_nonBlockingEnabled = false;
		}
	}

	template <typename Lang>
	static inline TicketManager<Lang> &getTicketManager()
	{
		static TicketManager<Lang> _ticketManager;
		return _ticketManager;
	}

private:
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

#endif // ENVIRONMENT_HPP
