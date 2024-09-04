/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef COMPLETION_MANAGER_HPP
#define COMPLETION_MANAGER_HPP

#include <cstdint>

#include <boost/version.hpp>

// Support for BOOST 1.67
#if BOOST_VERSION == 106700
#include <boost/next_prior.hpp>
#endif

#include <boost/lockfree/spsc_queue.hpp>

#include "TaskContext.hpp"
#include "instrument/Instrument.hpp"
#include "util/EnvironmentVariable.hpp"
#include "util/ErrorHandler.hpp"


namespace tampi {

class CompletionManager {
private:
	static constexpr size_t Size = 32*1024;

	typedef boost::lockfree::spsc_queue<TaskContext, boost::lockfree::capacity<Size> > queue_t;

	static EnvironmentVariable<bool> _enabled;

	static queue_t _queue;

public:
	CompletionManager() = delete;
	CompletionManager(const CompletionManager &) = delete;
	const CompletionManager& operator= (const CompletionManager &) = delete;

	static void transfer(const TaskContext *contexts, size_t count)
	{
		size_t pushed = _queue.push(contexts, count);
		if (pushed != count)
			ErrorHandler::fail("Failed to push task contexts");
	}

	static size_t process()
	{
		if (!_queue.read_available())
			return 0;

		Instrument::Guard<CompletedRequest> instrGuard;

		// Complete all task contexts
		return _queue.consume_all(
			[&](TaskContext &context) {
				context.completeEvents(1, true);
			});
	}

	static bool isEnabled()
	{
		return _enabled;
	}
};

} // namespace tampi

#endif // COMPLETION_MANAGER_HPP
