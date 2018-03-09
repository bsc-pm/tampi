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

#ifdef HAVE_NANOX_NANOS_H
#include <nanox/nanos.h>
#define nanos_get_current_task nanos_current_wd
#endif

#ifdef HAVE_NANOS6_H
#include <nanos6.h>
#endif

#include <array>
#include <cassert>


class TicketBase {
private:
	void *_waiter;
	int _pending;
	
public:
	TicketBase() :
		_waiter(),
		_pending(0)
	{
		_waiter = nanos_get_current_blocking_context();
	}
	
	TicketBase(const TicketBase &gt) = delete;
	TicketBase& operator=(const TicketBase &gt) = delete;
	
	~TicketBase()
	{
		assert(finished());
	}
	
	bool finished() const
	{
		return _pending == 0;
	}
	
	void addPendingRequest()
	{
		++_pending;
	}

	void removePendingRequest()
	{
		assert( _pending > 0 );
		--_pending;
	}
	
	int getPendingRequests() const
	{
		return _pending;
	}
	
	void notifyCompletion()
	{
		removePendingRequest();
		if (finished()) {
			nanos_unblock_task(_waiter);
		}
	}
	
	void wait()
	{
		nanos_block_current_task(_waiter);
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
	inline Ticket(Request &request, Status *status = MPI_STATUS_IGNORE)
		: TicketBase(),
		_firstRequest(&request),
		_firstStatus(status)
	{
	}
	
	inline Ticket(util::array_view<Request> &requests, Status *firstStatus = MPI_STATUSES_IGNORE) :
		TicketBase(),
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
	
	inline void notifyCompletion(int statusPos, const Status& status)
	{
		if (!ignoreStatus()) {
			const Status *first = &status;
			const Status *last  = std::next(first);
			Status *out = std::next(_firstStatus, statusPos);
			std::copy<const Status*,Status*>(first, last, out);
		}
		TicketBase::notifyCompletion();
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
	Ticket(Request &request, MPI_Fint *status = MPI_F_STATUS_IGNORE)
		: TicketBase(),
		_firstRequest(&request),
		_firstStatus(status)
	{
	}
	
	Ticket(util::array_view<Request> &requests, MPI_Fint *firstStatus = MPI_F_STATUSES_IGNORE)
		: TicketBase(),
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
	
	void notifyCompletion(int statusPos, const Status &status)
	{
		if (!ignoreStatus()) {
			const Status *first = &status;
			const Status *last  = std::next(first);
			Status* out = std::next((Status*)_firstStatus, statusPos);
			std::copy<const Status*,Status*>(first, last, out);
		}
		TicketBase::notifyCompletion();
	}
};
} // namespace Fortran

#endif // TICKET_H

