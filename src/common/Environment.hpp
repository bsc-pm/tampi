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


template <typename Lang>
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

#if !HAVE_BLOCKING_MODE
		blockingMode = false;
#endif
#if !HAVE_NONBLOCKING_MODE
		nonBlockingMode = false;
#endif

		_blockingEnabled = blockingMode;
		_nonBlockingEnabled = nonBlockingMode;

		Lang::initialize();
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

	static inline TicketManager<Lang> &getTicketManager()
	{
		static TicketManager<Lang> _ticketManager;
		return _ticketManager;
	}

private:
	static void polling(void *)
	{
		getTicketManager().checkRequests();
	}
};

template <typename Lang>
std::atomic<bool> Environment<Lang>::_blockingEnabled;
template <typename Lang>
std::atomic<bool> Environment<Lang>::_nonBlockingEnabled;
template <typename Lang>
TaskingModel::polling_handle_t Environment<Lang>::_pollingHandle;

#endif // ENVIRONMENT_HPP
