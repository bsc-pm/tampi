/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef TICKET_HPP
#define TICKET_HPP

#include <cassert>

#include "Interface.hpp"
#include "Operation.hpp"
#include "TaskContext.hpp"

namespace tampi {

//! Class that tracks a pending operation issued by a task
template <typename Lang>
class Ticket {
private:
	typedef typename Types<Lang>::status_t status_t;
	typedef typename Types<Lang>::status_ptr_t status_ptr_t;

	//! The context of the owner task
	TaskContext _taskContext;

	//! A pointer to the location where to save the statuses
	status_ptr_t _firstStatus;

public:
	//! \brief Construct an empty ticket
	Ticket() :
		_taskContext(),
		_firstStatus(nullptr)
	{
	}

	//! \brief Construct a ticket
	//!
	//! \param firstStatus A pointer to the location where to save
	//!                    the statuses or STATUS_IGNORE
	//! \param blocking Whether the TAMPI operation is blocking
	Ticket(status_ptr_t firstStatus, bool blocking) :
		_taskContext(blocking),
		_firstStatus(firstStatus)
	{
	}

	Ticket(const Operation<Lang> &operation) :
		_taskContext(operation._nature == BLK, operation._task),
		_firstStatus(operation._status)
	{
	}

	Ticket(const CollOperation<Lang> &operation) :
		_taskContext(operation._nature == BLK, operation._task),
		_firstStatus(Interface<Lang>::STATUS_IGNORE)
	{
	}

	//! \brief Add pending operation to the ticket
	//!
	//! \param num The number of pending operations to add
	void addPendingOperation(int num = 1)
	{
		_taskContext.bindEvents(num);
	}

	//! \brief Mark the ticket as completed
	void complete()
	{
		_taskContext.completeEvents(1, true);
	}

	//! \brief Get the task context
	const TaskContext &getTaskContext() const
	{
		return _taskContext;
	}

	//! \brief Wait until all operations complete
	//!
	//! This function blocks the current task until all operations
	//! complete. Note this is only valid for tickets of blocking
	//! TAMPI operations
	void wait()
	{
		_taskContext.waitEventsCompletion();
	}

	//! \brief Indicate whether the statuses should be ignored
	//!
	//! \returns Whether ignore the statuses
	bool ignoreStatus() const
	{
		return (_firstStatus == Interface<Lang>::STATUS_IGNORE
			|| _firstStatus == Interface<Lang>::STATUSES_IGNORE);
	}

	//! \brief Store the status of a request
	//!
	//! \param status The status to be stored
	//! \param statusPosition The local position in the array of statuses
	void storeStatus(const status_t& status, int statusPosition)
	{
		assert(_firstStatus != nullptr);
		assert(statusPosition >= 0);

		status_t *outStatus = (status_t *) _firstStatus;
		outStatus += statusPosition;
		std::copy(&status, &status + 1, outStatus);
	}
};

} // namespace tampi

#endif // TICKET_HPP

