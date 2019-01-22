/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef TICKET_MANAGER_HPP
#define TICKET_MANAGER_HPP

#include "util/array_view.hpp"
#include "util/spin_mutex.hpp"
#include "util/utils.hpp"

#include "Symbols.hpp"
#include "Ticket.hpp"
#include "TicketManagerInternals.hpp"

#include <cstring>
#include <mutex>
#include <vector>

#include <mpi.h>

template <typename Lang>
class TicketManager {
private:
	const static int NENTRIES = 32*1000;
	
	typedef typename Lang::request_t request_t;
	typedef typename Lang::status_t  status_t;
	typedef typename Lang::status_ptr_t status_ptr_t;
	typedef ::Ticket<Lang> Ticket;
	
	volatile int _pending;
	TicketManagerInternals<Lang> _arrays;
	
	int *_indices;
	mutable util::spin_mutex _mutex;
	
public:
	TicketManager()
		: _pending(0),
		_arrays(NENTRIES),
		_indices(nullptr),
		_mutex()
	{
		_indices = (int *) std::malloc(NENTRIES * sizeof(int));
		assert(_indices != nullptr);
	}
	
	~TicketManager()
	{
		assert(_indices != nullptr);
		std::free(_indices);
	}
	
	TicketManager(const TicketManager &) = delete;
	
	const TicketManager& operator= (const TicketManager &) = delete;

	template<typename... Args>
	void add(Args &&... args)
	{
		do {
			while (_pending >= NENTRIES) util::spinWait();
		} while (!tryAdd(std::forward<Args>(args)...));
	}
	
	inline void completeRequest(unsigned int position, const status_t &status)
	{
		int localPosition = _arrays.getLocalPositionInTicket(position);
		Ticket &ticket = _arrays.getAssociatedTicket(position);
		ticket.requestCompleted(status, localPosition);
	}
	
	void checkRequests()
	{
		std::lock_guard<util::spin_mutex> guard(_mutex);
		
		bool retry = true;
		while (_pending > 0 && retry) {
			retry = innerCheckRequests();
		}
	}
	
private:
	bool innerCheckRequests();
	
	bool tryAdd(request_t &request, Ticket &ticket);
	
	bool tryAdd(util::array_view<request_t> &requests, Ticket &ticket);
	
	template<typename... Args>
	bool tryAdd(request_t &request, Args &&... ticketArgs);
	
	template<typename... Args>
	bool tryAdd(util::array_view<request_t> &requests, Args &&... ticketArgs);
};

template <>
inline bool TicketManager<C>::innerCheckRequests()
{
	assert(_pending > 0);
	
	int completed = 0;
	int err = PMPI_Testsome(_pending, _arrays.getRequests(), &completed, _indices, _arrays.getStatuses());
	if (err != MPI_SUCCESS)
		error::fail("Unexpected return code from MPI_Testsome");
	
	if (completed == MPI_UNDEFINED) {
		/* Abort the program since this case should never occur.
 		 * The number of completed MPI requests is MPI_UNDEFINED
 		 * when all passed requests were already inactive */
		error::fail("Unexpected output from PMPI_Testsome");
	} else if (completed > 0) {
		int replacement = _pending - 1;
		int reverse = completed - 1;
		
		for (int c = 0; c < completed; ++c) {
			const int current = _indices[c];
			completeRequest(current, _arrays.getStatus(c));
			
			bool replace = false;
			while (replacement > current) {
				if (replacement != _indices[reverse]) {
					replace = true;
					break;
				}
				--replacement;
				--reverse;
			}
			
			if (replace) {
				_arrays.replaceRequest(replacement, current);
				--replacement;
			}
		}
		_pending -= completed;
	}
	return (completed > 0);
}

template <>
inline bool TicketManager<Fortran>::innerCheckRequests()
{
	assert(_pending > 0);
	
	int err, count = _pending;
	int completed = 0;
	pmpi_testsome_(&count, _arrays.getRequests(), &completed, _indices, _arrays.getStatuses(), &err);
	if (err != MPI_SUCCESS)
		error::fail("Unexpected return code from MPI_Testsome");
	
	if (completed == MPI_UNDEFINED) {
		/* Abort the program since this case should never occur.
 		 * The number of completed MPI requests is MPI_UNDEFINED
 		 * when all passed requests were already inactive */
		error::fail("Unexpected output from MPI_Testsome");
	} else if (completed > 0) {
		int replacement = _pending - 1;
		int reverse = completed - 1;
		
		for (int c = 0; c < completed; ++c) {
			const int current = _indices[c] - 1;
			completeRequest(current, _arrays.getStatus(c));
			
			bool replace = false;
			while (replacement > current) {
				if (replacement != _indices[reverse] - 1) {
					replace = true;
					break;
				}
				--replacement;
				--reverse;
			}
			
			if (replace) {
				_arrays.replaceRequest(replacement, current);
				--replacement;
			}
		}
		_pending -= completed;
	}
	return (completed > 0);
}

template <typename Lang>
inline bool TicketManager<Lang>::tryAdd(request_t &request, Ticket &ticket)
{
	assert(request != Lang::REQUEST_NULL);
	std::lock_guard<util::spin_mutex> guard(_mutex);
	
	if (_pending < NENTRIES) {
		ticket.addPendingRequest();
		
		_arrays.associateRequest(_pending, request, ticket);
		
		++_pending;
		return true;
	}
	return false;
}

template <typename Lang>
inline bool TicketManager<Lang>::tryAdd(util::array_view<request_t> &requests, Ticket &ticket)
{
	assert(requests.size() > 0);
	std::lock_guard<util::spin_mutex> guard(_mutex);
	
	if (_pending + requests.size() <= NENTRIES) {
		int current = _pending;
		for (int r = 0; r < requests.size(); ++r) {
			if (requests[r] != Lang::REQUEST_NULL) {
				_arrays.associateRequest(current, requests[r], ticket, r);
				++current;
			}
		}
		
		assert(current > _pending);
		ticket.addPendingRequest(current - _pending);
		_pending = current;
		
		return true;
	}
	return false;
}

template <typename Lang> template<typename... Args>
inline bool TicketManager<Lang>::tryAdd(request_t &request, Args &&... ticketArgs)
{
	assert(request != Lang::REQUEST_NULL);
	std::lock_guard<util::spin_mutex> guard(_mutex);
	
	if (_pending < NENTRIES) {
		Ticket &ticket = _arrays.allocateTicket(_pending, std::forward<Args>(ticketArgs)...);
		ticket.addPendingRequest();
		
		_arrays.associateRequest(_pending, request, ticket);
		
		++_pending;
		return true;
	}
	return false;
}

template <typename Lang> template<typename... Args>
inline bool TicketManager<Lang>::tryAdd(util::array_view<request_t> &requests, Args &&... ticketArgs)
{
	assert(requests.size() > 0);
	std::lock_guard<util::spin_mutex> guard(_mutex);
	
	if (_pending + requests.size() <= NENTRIES) {
		int current = _pending;
		for (int r = 0; r < requests.size(); ++r) {
			if (requests[r] != Lang::REQUEST_NULL) {
				Ticket &ticket = _arrays.allocateTicket(current, std::forward<Args>(ticketArgs)...);
				ticket.addPendingRequest();
				
				_arrays.associateRequest(current, requests[r], ticket, r);
				++current;
			}
		}
		
		assert(current > _pending);
		_pending = current;
		
		return true;
	}
	return false;
}

#endif // TICKET_MANAGER_HPP
