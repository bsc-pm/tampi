/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef TICKET_H
#define TICKET_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "array_view.h"
#include "runtime_api.h"

#include <array>
#include <cassert>

class TicketBase {
private:
	void *_taskContextOrCounter;
	int _pending;
	bool _blockable;
	
public:
	TicketBase(bool isBlockable) :
		_taskContextOrCounter(nullptr),
		_pending(0),
		_blockable(isBlockable)
	{
		if (isBlockable) {
			_taskContextOrCounter = nanos_get_current_blocking_context();
		} else {
			_taskContextOrCounter = nanos_get_current_event_counter();
		}
		assert(_taskContextOrCounter != nullptr);
	}
	
	TicketBase(const TicketBase &gt) = delete;
	TicketBase& operator=(const TicketBase &gt) = delete;
	
	~TicketBase()
	{
		assert(_pending == 0);
	}

	bool finished() const
	{
		return _pending == 0;
	}
	
	bool isBlockable() const
	{
		return _blockable;
	}
	
	void addPendingRequest()
	{
		++_pending;
		if (!_blockable) {
			nanos_increase_current_task_event_counter(_taskContextOrCounter, 1);
		}
	}
	
	bool notifyCompletion()
	{
		assert(_pending > 0);
		--_pending;
		
		// Only non-blockable tickets can be explicitly freed
		bool disposable = (!_blockable && _pending == 0);
		
		if (!_blockable) {
			nanos_decrease_task_event_counter(_taskContextOrCounter, 1);
		} else if (_pending == 0) {
			// NOTE: Do NOT access to this ticket after this statement,
			// since the blocked task holds it in its stack
			nanos_unblock_task(_taskContextOrCounter);
		}
		
		return disposable;
	}
	
	void wait()
	{
		assert(_blockable);
		nanos_block_current_task(_taskContextOrCounter);
	}
};

namespace C {
struct Ticket : public TicketBase {
	typedef MPI_Request Request;
	typedef MPI_Status  Status;
	
private:
	Request *_firstRequest;
	Status *_firstStatus;
	
public:
	inline Ticket(Request &request, Status *status, bool blockable)
		: TicketBase(blockable),
		_firstRequest(&request),
		_firstStatus(status)
	{
	}
	
	inline Ticket(util::array_view<Request> &requests, Status *firstStatus, bool blockable) :
		TicketBase(blockable),
		_firstRequest(requests.begin()),
		_firstStatus(firstStatus)
	{
	}
	
	inline Status* getStatus()
	{
		return _firstStatus;
	}
	
	inline bool ignoreStatus() const
	{
		return _firstStatus == MPI_STATUS_IGNORE || _firstStatus == MPI_STATUSES_IGNORE;
	}
	
	inline bool notifyCompletion(int statusPos, const Status& status)
	{
		if (!ignoreStatus()) {
			const Status *first = &status;
			const Status *last  = std::next(first);
			Status *out = std::next(_firstStatus, statusPos);
			std::copy<const Status*,Status*>(first, last, out);
		}
		return TicketBase::notifyCompletion();
	}
};
} // namespace C

namespace Fortran {
struct Ticket : public TicketBase {
	typedef MPI_Fint Request;
	typedef std::array<MPI_Fint, sizeof(MPI_Status)/sizeof(MPI_Fint)> Status;
	
private:
	Request *_firstRequest;
	MPI_Fint *_firstStatus;
	
public:
	Ticket(Request &request, MPI_Fint *status, bool blockable)
		: TicketBase(blockable),
		_firstRequest(&request),
		_firstStatus(status)
	{
	}
	
	Ticket(util::array_view<Request> &requests, MPI_Fint *firstStatus, bool blockable)
		: TicketBase(blockable),
		_firstRequest(requests.begin()),
		_firstStatus(firstStatus)
	{
	}
	
	MPI_Fint* getStatus()
	{
		return _firstStatus;
	}
	
	bool ignoreStatus() const
	{
		// MPI fortran API decays status array to a pointer to integer
		return _firstStatus == MPI_F_STATUS_IGNORE || _firstStatus == MPI_F_STATUSES_IGNORE;
	}
	
	bool notifyCompletion(int statusPos, const Status &status)
	{
		if (!ignoreStatus()) {
			const Status *first = &status;
			const Status *last  = std::next(first);
			Status* out = std::next((Status*)_firstStatus, statusPos);
			std::copy<const Status*,Status*>(first, last, out);
		}
		return TicketBase::notifyCompletion();
	}
};
} // namespace Fortran

#endif // TICKET_H

