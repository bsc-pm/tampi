/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef TICKET_HPP
#define TICKET_HPP

#include "Definitions.hpp"
#include "TaskContext.hpp"

#include <cassert>

#include <mpi.h>

template <typename Lang>
class Ticket {
private:
	typedef typename Lang::status_t status_t;
	typedef typename Lang::status_ptr_t status_ptr_t;
	
	int _pending;
	TaskContext _taskContext;
	status_ptr_t _firstStatus;
	
public:
	inline Ticket(status_ptr_t firstStatus, bool blocking) :
		_pending(0),
		_taskContext(blocking),
		_firstStatus(firstStatus)
	{}
	
	inline ~Ticket()
	{
		assert(_pending == 0);
	}
	
	inline bool finished() const
	{
		return _pending == 0;
	}
	
	inline bool ignoreStatus() const
	{
		return (_firstStatus == Lang::STATUS_IGNORE || _firstStatus == Lang::STATUSES_IGNORE);
	}
	
	inline void addPendingRequest(int num = 1)
	{
		_pending += num;
		_taskContext.bindedEvents(num);
	}
	
	inline void requestCompleted(const status_t& status, int statusPosition)
	{
		assert(_pending > 0);
		--_pending;
		
		if (!ignoreStatus()) {
			storeStatus(status, statusPosition);
		}
		
		_taskContext.completedEvents(1, _pending == 0);
	}
	
	inline void wait()
	{
		_taskContext.waitEventsCompletion();
	}
	
	inline void storeStatus(const status_t& status, int statusPosition)
	{
		assert(_firstStatus != nullptr);
		assert(statusPosition >= 0);
		
		status_t *outStatus = (status_t *) _firstStatus;
		outStatus += statusPosition;
		std::copy(&status, &status+1, outStatus);
	}
};

#endif // TICKET_HPP

