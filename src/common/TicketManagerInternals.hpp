/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef TICKET_MANAGER_INTERNALS_HPP
#define TICKET_MANAGER_INTERNALS_HPP

#include "Definitions.hpp"
#include "Ticket.hpp"

#include <cassert>
#include <cstdlib>
#include <utility>

template <typename Lang>
class TicketManagerInternals {
private:
	typedef typename Lang::request_t request_t;
	typedef typename Lang::status_t status_t;
	typedef typename Lang::status_ptr_t status_ptr_t;
	typedef ::Ticket<Lang> Ticket;
	
	struct mapping_t 
	{
		Ticket *ticket;
		int localPosition;
	};
	
	int _capacity;
	request_t *_requests;
	status_t  *_statuses;
	mapping_t *_mappings;
	Ticket    *_tickets;
	
public:
	TicketManagerInternals(int capacity)
		: _capacity(capacity)
	{
		assert(capacity > 0);
		_requests = (request_t *) std::malloc(capacity * sizeof(request_t));
		_statuses =  (status_t *) std::malloc(capacity * sizeof(status_t));
		_mappings = (mapping_t *) std::malloc(capacity * sizeof(mapping_t));
		_tickets  =    (Ticket *) std::malloc(capacity * sizeof(Ticket));
		assert(_requests && _statuses && _mappings && _tickets);
	}
	
	~TicketManagerInternals()
	{
		assert(_requests && _statuses && _mappings && _tickets);
		std::free(_requests);
		std::free(_statuses);
		std::free(_mappings);
		std::free(_tickets);
	}
	
	inline int capacity() const
	{
		return _capacity;
	}
	
	inline const request_t &getRequest(int position) const
	{
		return _requests[position];
	}
	
	inline request_t &getRequest(int position)
	{
		return _requests[position];
	}
	
	inline const status_t &getStatus(int position) const
	{
		return _statuses[position];
	}
	
	inline status_t &getStatus(int position)
	{
		return _statuses[position];
	}
	
	inline request_t *getRequests() const
	{
		return _requests;
	}
	
	inline status_ptr_t getStatuses() const
	{
		return (status_ptr_t) _statuses;
	}
	
	inline const Ticket &getAssociatedTicket(int position) const
	{
		assert(_mappings[position].ticket != nullptr);
		return *_mappings[position].ticket;
	}
	
	inline Ticket &getAssociatedTicket(int position)
	{
		assert(_mappings[position].ticket != nullptr);
		return *_mappings[position].ticket;
	}
	
	inline int getLocalPositionInTicket(int position) const
	{
		assert(_mappings[position].localPosition >= 0);
		return _mappings[position].localPosition;
	}
	
	template<typename... Args>
	inline Ticket &allocateTicket(int position, Args &&... ticketArgs)
	{
		new (&_tickets[position]) Ticket(std::forward<Args>(ticketArgs)...);
		return _tickets[position];
	}
	
	inline void associateRequest(int position, request_t &request, Ticket &ticket, int localPosition = 0)
	{
		_requests[position] = request;
		_mappings[position].ticket = &ticket;
		_mappings[position].localPosition = localPosition;
	}
	
	inline void replaceRequest(int source, int destination)
	{
		assert(source != destination);
		_requests[destination] = _requests[source];
		_mappings[destination] = _mappings[source];
		
		if (_mappings[source].ticket == &_tickets[source]) {
			_tickets[destination] = _tickets[source];
			_mappings[destination].ticket = &_tickets[destination];
		}
	}
};

#endif // TICKET_MANAGER_INTERNALS_HPP
