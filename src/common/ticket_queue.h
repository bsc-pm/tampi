/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef TICKET_QUEUE_H
#define TICKET_QUEUE_H

#include "array_view.h"
#include "spin_mutex.h"
#include "symbols.h"
#include "ticket.h"

#include <mutex>
#include <vector>

#include <mpi.h>

template < typename Ticket >
class TicketQueue {
private:
	typedef typename Ticket::Request Request;
	typedef typename Ticket::Status  Status;
	
	struct TicketMapping {
		Ticket* ticket;
		int requestPosition;
		
		TicketMapping(Ticket &t, int pos = 0)
			: ticket(&t),
			requestPosition(pos)
		{
		}
	};
	
	std::vector<Request> _requests;
	std::vector<Status> _statuses;
	std::vector<TicketMapping> _tickets;
	mutable spin_mutex _mutex;
	
public:
	TicketQueue()
		: _requests(),
		_statuses(),
		_tickets(),
		_mutex()
	{
	}
	
	TicketQueue(const TicketQueue &) = delete;
	
	const TicketQueue& operator= (const TicketQueue &) = delete;
	
	void add(Ticket &ticket, Request &req);
	
	void add(Ticket &ticket, util::array_view<Request> &requests);
	
	void poll();
	
	void requestCompleted(int position) {
		auto reqIt  = std::next(_requests.begin(),position);
		auto statIt = std::next(_statuses.begin(),position);
		auto tmIt   = std::next(_tickets.begin(),position);
		
		TicketMapping tm = *tmIt;
		tm.ticket->notifyCompletion(tm.requestPosition, _statuses[position]);
		
		_requests.erase(reqIt);
		_statuses.erase(statIt);
		_tickets.erase(tmIt);
	}
};

template<>
inline void TicketQueue<C::Ticket>::add(C::Ticket &ticket, C::Ticket::Request &request)
{
	std::lock_guard<spin_mutex> guard(_mutex);
	ticket.addPendingRequest();
	_requests.push_back(request);
	_statuses.emplace_back();
	_tickets.emplace_back(ticket);
}

template<>
inline void TicketQueue<Fortran::Ticket>::add(Fortran::Ticket &ticket, Fortran::Ticket::Request &request)
{
	std::lock_guard<spin_mutex> guard(_mutex);
	ticket.addPendingRequest();
	_requests.push_back(request);
	_statuses.emplace_back();
	_tickets.emplace_back(ticket);
}

template<>
inline void TicketQueue<C::Ticket>::add(C::Ticket &ticket, util::array_view<C::Ticket::Request> &requests)
{
	int count = requests.size();
	
	std::lock_guard<spin_mutex> guard(_mutex);
	
	// Amortize cost of realloc when not enough capacity
	const size_t capacity = _requests.capacity();
	const size_t requiredCapacity = _requests.size() + count;
	if (requiredCapacity > capacity) {
		_requests.reserve(2 * requiredCapacity);
		_statuses.reserve(2 * requiredCapacity);
		_tickets.reserve (2 * requiredCapacity);
	}
	
	// Insert each uncompleted request
	// (requests not present in indices array)
	// only when its value is not MPI_REQUEST_NULL
	// (MPI_Testsome does not report inactive requests
	// as completed)
	for (int u = 0; u < count; ++u) {
		C::Ticket::Request& request = requests[u];
		ticket.addPendingRequest();
		_requests.push_back(request);
		_statuses.emplace_back();
		_tickets.emplace_back(ticket, u);
	}
}

template<>
inline void TicketQueue<Fortran::Ticket>::add(Fortran::Ticket &ticket, util::array_view<Fortran::Ticket::Request> &requests)
{
	MPI_Fint count = requests.size();
	
	std::lock_guard<spin_mutex> guard(_mutex);
	
	// Amortize cost of realloc when not enough capacity
	const size_t capacity = _requests.capacity();
	const size_t requiredCapacity = _requests.size() + count;
	if (requiredCapacity > capacity) {
		_requests.reserve(2 * requiredCapacity);
		_statuses.reserve(2 * requiredCapacity);
		_tickets.reserve (2 * requiredCapacity);
	}
	
	// Insert each uncompleted request
	// (requests not present in indices array)
	// only when its value is not MPI_REQUEST_NULL
	// (MPI_Testsome does not report inactive requests
	// as completed)
	for (int u = 0; u < count; ++u) {
		Fortran::Ticket::Request& request = requests[u];
		ticket.addPendingRequest();
		_requests.push_back(request);
		_statuses.emplace_back();
		_tickets.emplace_back(ticket, u);
	}
}

template<>
inline void TicketQueue<C::Ticket>::poll() {
	if (!_requests.empty()) {
		std::unique_lock<spin_mutex> guard(_mutex, std::try_to_lock);
		
		if (guard.owns_lock() && !_requests.empty()) {
			int count = _requests.size();
			int completed = 0;
			int indices[count];
			PMPI_Testsome(count, _requests.data(), &completed, indices, _statuses.data());
			
			if (completed == MPI_UNDEFINED) {
				// All requests were already released.
				// Consider skip deletion of completed requests and only clear
				// them whenever all of them are released.
				_requests.clear();
				_statuses.clear();
				_tickets.clear();
			} else {
				// Assumes indices array is sorted from lower to higher values
				// Iterate from the end to the beginning of the array to ensure
				// indexed element positions are not changed after a deletion
				for (int index = completed; index > 0; index--) {
					requestCompleted(indices[index-1]);
				}
			}
		}
	}
}

template<>
inline void TicketQueue<Fortran::Ticket>::poll() {
	if (!_requests.empty()) {
		std::unique_lock<spin_mutex> guard(_mutex, std::try_to_lock);
		
		if (guard.owns_lock() && !_requests.empty()) {
			int count = _requests.size();
			int completed = 0;
			int indices[count];
			int err = MPI_SUCCESS;
			
			pmpi_testsome_(&count, _requests.data(), &completed, indices, (MPI_Fint *)_statuses.data(), &err);
			
			if (completed == MPI_UNDEFINED) {
				// All requests are already released.
				// Consider skip deletion of completed requests and only clear
				// them whenever all of them are released.
				_requests.clear();
				_statuses.clear();
				_tickets.clear();
			} else {
				// Assumes indices array is sorted from lower to higher values
				// Iterate from the end to the beginning of the array to ensure
				// indexed element positions are not changed after a deletion
				for (int index = completed; index > 0; index--) {
					requestCompleted(indices[index-1] - 1);
				}
			}
		}
	}
}

#endif // TICKET_QUEUE_H
