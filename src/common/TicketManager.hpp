/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2022 Barcelona Supercomputing Center (BSC)
*/

#ifndef TICKET_MANAGER_HPP
#define TICKET_MANAGER_HPP

#include <mpi.h>

#include <algorithm>
#include <cstring>
#include <functional>
#include <mutex>
#include <vector>

#include "Ticket.hpp"
#include "TicketManagerInternals.hpp"
#include "util/ArrayView.hpp"
#include "util/LockFreeQueue.hpp"
#include "util/SpinLock.hpp"
#include "util/Utils.hpp"


namespace tampi {

//! Class that manages all the tickets and its corresponding requests
//! created by tasks. It provides functions to efficiently add new
//! tickets and requests to the system, and also, it defines functions
//! to check their progress. Tasks are notified when any of their
//! requests complete
template <typename Lang>
class TicketManager {
private:
	//! Maximum number of in-flight requests
	static const int NENTRIES = 32*1000;

	//! Maximum number of requests to be inserted at once
	static const int NRPG = 64;

	typedef typename Lang::request_t request_t;
	typedef typename Lang::status_t status_t;
	typedef typename Lang::status_ptr_t status_ptr_t;
	typedef tampi::Ticket<Lang> Ticket;
	typedef std::function<void()> ProgressFunction;

	//! Structure representing a blocking request in the pre-queues
	struct BlockingEntry {
		request_t _request;
		Ticket *_ticket;
		int _position;

		inline BlockingEntry(const request_t &request, Ticket &ticket, int position = 0) :
			_request(request),
			_ticket(&ticket),
			_position(position)
		{
		}

		inline BlockingEntry() :
			_request(Lang::REQUEST_NULL),
			_ticket(nullptr),
			_position(0)
		{
		}
	};

	//! Structure representing a non-blocking request in the pre-queues
	struct NonBlockingEntry {
		request_t _request;
		Ticket _ticket;
		int _position;

		inline NonBlockingEntry(
			const request_t &request,
			Ticket &ticket,
			int position = 0
		) :
			_request(request),
			_ticket(ticket),
			_position(position)
		{
		}

		inline NonBlockingEntry() :
			_request(Lang::REQUEST_NULL),
			_ticket(),
			_position(0)
		{
		}
	};

	//! Number of in-flight requests
	int _pending;

	//! General arrays storing the tickets, requests and statuses
	TicketManagerInternals<Lang, NENTRIES> _arrays;

	//! Lock-free pre-queues of blocking requests
	util::LockFreeQueue<BlockingEntry> _blockingEntries;

	//! Lock-free pre-queues of non-blocking requests
	util::LockFreeQueue<NonBlockingEntry> _nonBlockingEntries;

	//! Progression function when failing to insert requests in pre-queues
	ProgressFunction _checkEntriesFunc;

	//! Auxiliary array of indices used when checking requests
	int *_indices;

	//! Spinlock for consuming requests from pre-queues and checking in-flight
	//! requests from the general array
	mutable SpinLock _mutex;

public:
	TicketManager() :
		_pending(0),
		_arrays(),
		_blockingEntries(),
		_nonBlockingEntries(),
		_checkEntriesFunc(),
		_indices(nullptr),
		_mutex()
	{
		_indices = (int *) std::malloc(NENTRIES * sizeof(int));
		assert(_indices != nullptr);

		_checkEntriesFunc = std::bind(&TicketManager::checkEntryQueues, this);
	}

	~TicketManager()
	{
		assert(_indices != nullptr);
		std::free(_indices);
	}

	TicketManager(const TicketManager &) = delete;
	const TicketManager& operator= (const TicketManager &) = delete;

	//! \brief Complete a request from the general array
	//!
	//! \param position The position of the request in the array
	//! \param status The resulting MPI status of the operation
	inline void completeRequest(unsigned int position, const status_t &status)
	{
		int localPosition = _arrays.getLocalPositionInTicket(position);
		Ticket &ticket = _arrays.getAssociatedTicket(position);
		ticket.requestCompleted(status, localPosition);
	}

	//! \brief Check the in-flight requests
	//!
	//! This function first transfers all requests from the pre-queues to the
	//! general array of requests. Then, it checks all requests from that
	//! array. Once a request completes, the counter of pending requests of
	//! the owner task is decreased. This will trigger the resume of the task
	//! or the fulfillment of its external event if that was the last request
	inline void checkRequests()
	{
		std::lock_guard<SpinLock> guard(_mutex);

		if (_pending < NENTRIES) {
			internalCheckEntryQueues();
		}

		bool retry = true;
		while (_pending > 0 && retry) {
			retry = internalCheckRequests();
		}
	}

	//! \brief Add a request and its ticket to the pre-queues
	//!
	//! This function adds the request to the pre-queues so that it can be
	//! later processed and tranferred to the global array of requests and
	//! tickets. In case the ticket is blocking, the ticket cannot be destroyed
	//! until the request completes. The ticket must be wait on before
	//! destroying. Otherwise, in case it is non-blocking, the ticket should
	//! not be waited on and can be destroyed after returning.
	//!
	//! \param request The request to add
	//! \param ticket The ticket which the request belongs to
	void addRequest(request_t &request, Ticket &ticket);

	//! \brief Add multiple requests and their ticket to the pre-queues
	//!
	//! This function adds the requests to the pre-queues so that they can be
	//! later processed and tranferred to the global array of requests and
	//! tickets. In case the ticket is blocking, the ticket cannot be destroyed
	//! until the requests complete. The ticket must be wait on before
	//! destroying. Otherwise, in case it is non-blocking, the ticket should
	//! not be waited on and can be destroyed after returning.
	//!
	//! \param request The requests to add
	//! \param ticket The ticket which the requests belongs to
	void addRequests(util::ArrayView<request_t> &requests, Ticket &ticket);

private:
	//! \brief Internal function to check the requests that are in-flight
	//!
	//! \returns Whether any request completed
	bool internalCheckRequests();

	//! \brief Check and transfer requests from pre-queues
	//!
	//! This function tries to acquire the lock before trying to
	//! transfer requests from pre-queues to the general array
	inline void checkEntryQueues()
	{
		std::unique_lock<SpinLock> guard(_mutex, std::try_to_lock);
		if (guard.owns_lock()) {
			internalCheckEntryQueues();
		}
	}

	//! \brief Add multiple requests and their ticket to a specific pre-queue
	//!
	//! \param request The requests to add
	//! \param ticket The ticket which the requests belongs to
	template <typename EntryTy>
	void addRequests(util::ArrayView<request_t> &requests, Ticket &ticket, util::LockFreeQueue<EntryTy> &queue);

	//! \brief Internal function to check and transfer requests from pre-queues
	//!
	//! This function assumes the lock is already acquired
	void internalCheckEntryQueues();

	//! \brief Transfers blocking requests to the general array
	//!
	//! \param entries The array of entries containing the requests to transfer
	//! \param count The number of requests to add
	void transferEntries(BlockingEntry entries[], int count);

	//! \brief Transfers non-blocking requests to the general array
	//!
	//! \param entries The array of entries containing the requests to transfer
	//! \param count The number of requests to add
	void transferEntries(NonBlockingEntry entries[], int count);

	//! \brief Get the number of active requests in an array
	//!
	//! \param requests The array of requests
	//!
	//! \returns The number of active requests
	static inline int getActiveRequestCount(const util::ArrayView<request_t> &requests)
	{
		int active = 0;
		for (int r = 0; r < requests.size(); ++r) {
			if (requests[r] != Lang::REQUEST_NULL) {
				++active;
			}
		}
		return active;
	}
};

template <>
inline bool TicketManager<C>::internalCheckRequests()
{
	assert(_pending > 0);

	int completed = 0;
	int err = PMPI_Testsome(_pending, _arrays.getRequests(), &completed, _indices, _arrays.getStatuses());
	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Unexpected return code from MPI_Testsome");

	if (completed == MPI_UNDEFINED) {
		// Abort the program since this case should never occur.
		// The number of completed MPI requests is MPI_UNDEFINED
		// when all passed requests were already inactive
		ErrorHandler::fail("Unexpected output from MPI_Testsome");
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
				_arrays.moveRequest(replacement, current);
				--replacement;
			}
		}
		_pending -= completed;
	}
	return (completed > 0);
}

template <>
inline bool TicketManager<Fortran>::internalCheckRequests()
{
	assert(_pending > 0);

	int err, count = _pending;
	int completed = 0;
	pmpi_testsome_(&count, _arrays.getRequests(), &completed, _indices, _arrays.getStatuses(), &err);
	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Unexpected return code from MPI_Testsome");

	if (completed == MPI_UNDEFINED) {
		// Abort the program since this case should never occur.
		// The number of completed MPI requests is MPI_UNDEFINED
		// when all passed requests were already inactive
		ErrorHandler::fail("Unexpected output from MPI_Testsome");
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
				_arrays.moveRequest(replacement, current);
				--replacement;
			}
		}
		_pending -= completed;
	}
	return (completed > 0);
}

template <typename Lang>
inline void TicketManager<Lang>::addRequest(request_t &request, Ticket &ticket)
{
	assert(request != Lang::REQUEST_NULL);

	ticket.addPendingRequest();

	if (ticket.isBlocking()) {
		BlockingEntry entry(request, ticket);
		_blockingEntries.add(entry, _checkEntriesFunc);
	} else {
		NonBlockingEntry entry(request, ticket);
		_nonBlockingEntries.add(entry, _checkEntriesFunc);
	}
}

template <typename Lang>
inline void TicketManager<Lang>::addRequests(util::ArrayView<request_t> &requests, Ticket &ticket)
{
	if (ticket.isBlocking()) {
		addRequests(requests, ticket, _blockingEntries);
	} else {
		addRequests(requests, ticket, _nonBlockingEntries);
	}
}

template <typename Lang>
template <typename EntryTy>
inline void TicketManager<Lang>::addRequests(util::ArrayView<request_t> &requests, Ticket &ticket, util::LockFreeQueue<EntryTy> &queue)
{
	util::Uninitialized<EntryTy> entries[NRPG];

	const int active = getActiveRequestCount(requests);
	assert(active > 0);

	ticket.addPendingRequest(active);

	int req = 0;
	for (int added = 0; added < active; added += NRPG) {
		const int size = std::min(NRPG, active - added);
		int entry = 0;
		while (entry < size) {
			if (requests[req] != Lang::REQUEST_NULL) {
				new (&entries[entry]) EntryTy(requests[req], ticket, req);
				++entry;
			}
			++req;
		}
		queue.add((EntryTy *)entries, size, _checkEntriesFunc);
	}
}

template <typename Lang>
inline void TicketManager<Lang>::internalCheckEntryQueues()
{
	util::Uninitialized<BlockingEntry> blockings[NRPG];
	util::Uninitialized<NonBlockingEntry> nonBlockings[NRPG];

	const int numAvailable = (NENTRIES - _pending);
	int numBlk, numNonBlk;
	int total = 0;

	do {
		numBlk = std::min(numAvailable - total, NRPG);
		numBlk = _blockingEntries.retrieve((BlockingEntry *)blockings, numBlk);
		if (numBlk > 0) {
			transferEntries((BlockingEntry *)blockings, numBlk);
			total += numBlk;
		}

		numNonBlk = std::min(numAvailable - total, NRPG);
		numNonBlk = _nonBlockingEntries.retrieve((NonBlockingEntry *)nonBlockings, numNonBlk);
		if (numNonBlk > 0) {
			transferEntries((NonBlockingEntry *)nonBlockings, numNonBlk);
			total += numNonBlk;
		}
	} while (total < numAvailable && (numBlk > 0 || numNonBlk > 0));
}

template <typename Lang>
inline void TicketManager<Lang>::transferEntries(BlockingEntry entries[], int count)
{
	assert(_pending + count <= NENTRIES);

	const int pending = _pending;
	for (int r = 0; r < count; ++r) {
		BlockingEntry &entry = entries[r];

		// All requests share the same ticket
		Ticket &ticket = *(entry._ticket);

		_arrays.associateRequest(pending + r,
				entry._request, ticket,
				entry._position);
	}
	_pending += count;
}

template <typename Lang>
inline void TicketManager<Lang>::transferEntries(NonBlockingEntry entries[], int count)
{
	assert(_pending + count <= NENTRIES);

	const int pending = _pending;
	for (int r = 0; r < count; ++r) {
		NonBlockingEntry &entry = entries[r];

		// Allocate a copy of the same ticket for each request
		Ticket &ticket = _arrays.allocateTicket(pending + r, entry._ticket);

		// Since there is a ticket per request, modify the number
		// of pending requests per ticket to one
		ticket.resetPendingRequests(1);

		_arrays.associateRequest(pending + r, entry._request, ticket, entry._position);
	}
	_pending += count;
}

template <typename Lang>
const int TicketManager<Lang>::NENTRIES;

template <typename Lang>
const int TicketManager<Lang>::NRPG;

} // namespace tampi

#endif // TICKET_MANAGER_HPP
