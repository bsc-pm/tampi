/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef TICKET_HPP
#define TICKET_HPP

#include <cassert>

#include "Interface.hpp"
#include "TaskContext.hpp"

namespace tampi {

//! Class that tracks the pending requests issued by a task
template <typename Lang>
class Ticket {
private:
	typedef typename Types<Lang>::status_t status_t;
	typedef typename Types<Lang>::status_ptr_t status_ptr_t;

	//! Number of pending requests assigned to the ticket
	int _pending;

	//! The context of the owner task
	TaskContext _taskContext;

	//! A pointer to the location where to save the statuses
	status_ptr_t _firstStatus;

public:
	//! \brief Construct an empty ticket
	Ticket() :
		_pending(0),
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
		_pending(0),
		_taskContext(blocking),
		_firstStatus(firstStatus)
	{
	}

	//! \brief Indicate whether the ticket is blocking
	bool isBlocking() const
	{
		return _taskContext.isBlocking();
	}

	//! \brief Get the number of pending requests
	int getPendingRequests() const
	{
		return _pending;
	}

	//! \brief Add pending requests to the ticket
	//!
	//! \param num The number of pending requests to add
	void addPendingRequests(int num)
	{
		_pending += num;
		_taskContext.bindEvents(num);
	}

	//! \brief Reset the pending requests of the ticket
	//!
	//! Notice this function does not alter the number of bound events
	void resetPendingRequests(int num)
	{
		_pending = num;
	}

	//! \brief Mark a request of the ticket as completed
	//!
	//! \param status The status of the completed request
	//! \param statusPosition The local position in the array of statuses
	void requestCompleted(const status_t& status, int statusPosition)
	{
		assert(_pending > 0);
		--_pending;

		if (!ignoreStatus()) {
			storeStatus(status, statusPosition);
		}

		_taskContext.completeEvents(1, _pending == 0);
	}

	//! \brief Wait until all requests complete
	//!
	//! This function blocks the current task until all requests
	//! complete. Note this is only valid for tickets of blocking
	//! TAMPI operations
	void wait()
	{
		_taskContext.waitEventsCompletion();
	}

private:
	//! \brief Indicate whether the statuses should be ignored
	//!
	//! \returns Whether ignore the statuses
	bool ignoreStatus() const
	{
		return (_firstStatus == Interface<Lang>::STATUS_IGNORE || _firstStatus == Interface<Lang>::STATUSES_IGNORE);
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
		std::copy(&status, &status+1, outStatus);
	}
};

} // namespace tampi

#endif // TICKET_HPP

