/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2022 Barcelona Supercomputing Center (BSC)
*/

#ifndef TICKET_MANAGER_INTERNALS_HPP
#define TICKET_MANAGER_INTERNALS_HPP

#include <cassert>
#include <cstdlib>
#include <utility>

#include "Ticket.hpp"


namespace tampi {

//! Class that provides storage for requests and tickets
template <typename Lang, size_t Capacity>
class TicketManagerInternals {
private:
	typedef typename Types<Lang>::request_t request_t;
	typedef typename Types<Lang>::status_t status_t;
	typedef typename Types<Lang>::status_ptr_t status_ptr_t;
	typedef tampi::Ticket<Lang> Ticket;

	//! Structure to map requests and tickets. Notice that
	//! a ticket may be associated to multiple requests
	struct Mapping {
		//! The associated request
		Ticket *ticket;

		//! The position of the request in the ticket
		int localPosition;
	};

	//! The array of requests
	request_t *_requests;

	//! The array of statuses
	status_t  *_statuses;

	//! The array of mappings between tickets and requests
	Mapping   *_mappings;

	//! The array of tickets
	Ticket    *_tickets;

public:
	inline TicketManagerInternals()
	{
		// Allocate all arrays
		_requests = (request_t *) std::malloc(Capacity * sizeof(request_t));
		_statuses =  (status_t *) std::malloc(Capacity * sizeof(status_t));
		_mappings =   (Mapping *) std::malloc(Capacity * sizeof(Mapping));
		_tickets  =    (Ticket *) std::malloc(Capacity * sizeof(Ticket));
		assert(_requests && _statuses && _mappings && _tickets);
	}

	inline ~TicketManagerInternals()
	{
		// Deallocate all arrays
		assert(_requests && _statuses && _mappings && _tickets);
		std::free(_requests);
		std::free(_statuses);
		std::free(_mappings);
		std::free(_tickets);
	}

	//! \brief Get the maximum capacity of the arrays
	//!
	//! \returns The capacity
	inline size_t capacity() const
	{
		return Capacity;
	}

	//! \brief Get a request from the array
	//!
	//! \param position The position of the request in the array
	//!
	//! \return The request
	inline const request_t &getRequest(int position) const
	{
		return _requests[position];
	}

	//! \brief Get a request from the array
	//!
	//! \param position The position of the request in the array
	//!
	//! \return The request
	inline request_t &getRequest(int position)
	{
		return _requests[position];
	}

	//! \brief Get the status of a request
	//!
	//! \param position The position of the request in the array
	//!
	//! \return The status
	inline const status_t &getStatus(int position) const
	{
		return _statuses[position];
	}

	//! \brief Get the status of a request
	//!
	//! \param position The position of the request in the array
	//!
	//! \return The status
	inline status_t &getStatus(int position)
	{
		return _statuses[position];
	}

	//! \brief Get the array of requests
	//!
	//! \return The array of requests
	inline request_t *getRequests() const
	{
		return _requests;
	}

	//! \brief Get the array of statuses
	//!
	//! \return The array of statuses
	inline status_ptr_t getStatuses() const
	{
		return (status_ptr_t) _statuses;
	}

	//! \brief Get the ticket associated to a request
	//!
	//! \param The position of the request in the array
	//!
	//! \return The ticket associated to the request
	inline const Ticket &getAssociatedTicket(int position) const
	{
		assert(_mappings[position].ticket != nullptr);
		return *_mappings[position].ticket;
	}

	//! \brief Get the ticket associated to a request
	//!
	//! \param The position of the request in the array
	//!
	//! \return The ticket associated to the request
	inline Ticket &getAssociatedTicket(int position)
	{
		assert(_mappings[position].ticket != nullptr);
		return *_mappings[position].ticket;
	}

	//! \brief Get the local position of the request in the ticket
	//!
	//! \param position The position of the request in the array
	//!
	//! \return The local position of the request in the ticket
	inline int getLocalPositionInTicket(int position) const
	{
		assert(_mappings[position].localPosition >= 0);
		return _mappings[position].localPosition;
	}

	//! \brief Allocate a ticket in the array
	//!
	//! \param position The position in the array to allocate the ticket
	//! \param ticketArgs The arguments to in-place construct the ticket
	//!
	//! \returns The allocated ticket
	template <typename... Args>
	inline Ticket &allocateTicket(int position, Args &&... ticketArgs)
	{
		new (&_tickets[position]) Ticket(std::forward<Args>(ticketArgs)...);
		return _tickets[position];
	}

	//! \brief Store and associate a request and a ticket
	//!
	//! \param position The position to store the request
	//! \param request The request
	//! \param ticket The ticket
	//! \param localPosition The local position of the request in the ticket
	inline void associateRequest(int position, request_t &request, Ticket &ticket, int localPosition = 0)
	{
		_requests[position] = request;
		_mappings[position].ticket = &ticket;
		_mappings[position].localPosition = localPosition;
	}

	//! \brief Move a request
	//!
	//! \param source The initial position of the request
	//! \param destination The new position of the request
	inline void moveRequest(int source, int destination)
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

} // namespace tampi

#endif // TICKET_MANAGER_INTERNALS_HPP
