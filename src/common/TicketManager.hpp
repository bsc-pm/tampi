/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef TICKET_MANAGER_HPP
#define TICKET_MANAGER_HPP

#include <mpi.h>

#include <algorithm>
#include <mutex>

#include "Interface.hpp"
#include "Ticket.hpp"
#include "TicketManagerInternals.hpp"
#include "instrument/Instrument.hpp"
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
	static constexpr int MaxEntries = 32*1024;

	//! Maximum number of requests to be inserted at once
	static constexpr int BatchSize = 64;

	typedef typename Types<Lang>::request_t request_t;
	typedef typename Types<Lang>::status_t status_t;
	typedef typename Types<Lang>::status_ptr_t status_ptr_t;
	typedef tampi::Ticket<Lang> Ticket;

	//! Structure representing a blocking request in the pre-queues
	struct BlockingEntry {
		request_t _request;
		Ticket *_ticket;
		int _position;

		BlockingEntry(const request_t &request, Ticket &ticket, int position = 0) :
			_request(request),
			_ticket(&ticket),
			_position(position)
		{
		}

		BlockingEntry() :
			_request(Interface<Lang>::REQUEST_NULL),
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

		NonBlockingEntry(
			const request_t &request,
			Ticket &ticket,
			int position = 0
		) :
			_request(request),
			_ticket(ticket),
			_position(position)
		{
		}

		NonBlockingEntry() :
			_request(Interface<Lang>::REQUEST_NULL),
			_ticket(),
			_position(0)
		{
		}
	};

	//! Number of in-flight requests
	int _pending;

	//! General arrays storing the tickets, requests and statuses
	TicketManagerInternals<Lang, MaxEntries> _arrays;

	//! Lock-free pre-queues of blocking requests
	LockFreeQueue<BlockingEntry> _blockingEntries;

	//! Lock-free pre-queues of non-blocking requests
	LockFreeQueue<NonBlockingEntry> _nonBlockingEntries;

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
		_indices(nullptr),
		_mutex()
	{
		_indices = (int *) std::malloc(MaxEntries * sizeof(int));
		assert(_indices != nullptr);
	}

	~TicketManager()
	{
		assert(_indices != nullptr);
		std::free(_indices);
	}

	TicketManager(const TicketManager &) = delete;
	const TicketManager& operator= (const TicketManager &) = delete;

	//! \brief Get the ticket manager for a specific language
	//!
	//! \returns A reference to the ticket manager
	static TicketManager &getTicketManager()
	{
		static TicketManager _ticketManager;
		return _ticketManager;
	}

	//! \brief Complete a request from the general array
	//!
	//! \param position The position of the request in the array
	//! \param status The resulting MPI status of the operation
	void completeRequest(unsigned int position, const status_t &status)
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
	//!
	//! \param pending add the number of pending requests
	//!
	//! \return the number of completed requests
	size_t checkRequests(size_t &pending)
	{
		std::lock_guard<SpinLock> guard(_mutex);

		if (_pending < MaxEntries) {
			internalCheckEntryQueues();
		}

		if (_pending == 0)
			return 0;

		size_t totalCompleted = 0;
		size_t completed = 0;
		do {
			completed = internalCheckRequests();
			totalCompleted += completed;
		} while (_pending > 0 && completed > 0);

		pending += _pending;

		return totalCompleted;
	}

	//! \brief Add a ticket and its request the pre-queues
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
	void addTicket(Ticket &ticket, request_t &request);

	//! \brief Add a ticket and their multiple requests to the pre-queues
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
	void addTicket(Ticket &ticket, ArrayView<request_t> &requests);

private:
	//! \brief Internal function to check the requests that are in-flight
	//!
	//! \returns Whether any request completed
	bool internalCheckRequests();

	//! \brief Check and transfer requests from pre-queues
	//!
	//! This function tries to acquire the lock before trying to
	//! transfer requests from pre-queues to the general array
	void checkEntryQueues()
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
	void addTicketRequests(
		Ticket &ticket,
		ArrayView<request_t> &requests,
		LockFreeQueue<EntryTy> &queue
	);

	//! \brief Internal function to check and transfer requests from pre-queues
	//!
	//! This function assumes the lock is already acquired
	void internalCheckEntryQueues();

	//! \brief Transfers blocking requests to the general array
	//!
	//! \param entries The array of entries containing the requests to transfer
	//! \param count The number of requests to add
	void transferEntries(BlockingEntry *entries, int count);

	//! \brief Transfers non-blocking requests to the general array
	//!
	//! \param entries The array of entries containing the requests to transfer
	//! \param count The number of requests to add
	void transferEntries(NonBlockingEntry *entries, int count);
};

template <typename Lang>
bool TicketManager<Lang>::internalCheckRequests()
{
	assert(_pending > 0);

	Instrument::Guard<CheckGlobalRequestArray> instrGuard;
	Instrument::enter<TestSomeRequests>();

	int completed = 0;
	int err = Interface<Lang>::testsome(_pending, _arrays.getRequests(), completed, _indices, _arrays.getStatuses());
	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Unexpected return code from MPI testsome");

	Instrument::exit<TestSomeRequests>();

	if (completed == MPI_UNDEFINED) {
		// Abort the program since this case should never occur.
		// The number of completed MPI requests is MPI_UNDEFINED
		// when all passed requests were already inactive
		ErrorHandler::fail("Unexpected output from MPI testsome");
	} else if (completed > 0) {
		int replacement = _pending - 1;
		int reverse = completed - 1;

		for (int c = 0; c < completed; ++c) {
			const int current = _indices[c];

			Instrument::enter<CompletedRequest>();
			completeRequest(current, _arrays.getStatus(c));
			Instrument::exit<CompletedRequest>();

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

template <typename Lang>
void TicketManager<Lang>::addTicket(Ticket &ticket, request_t &request)
{
	assert(request != Interface<Lang>::REQUEST_NULL);
	assert(ticket.getPendingRequests() == 1);

	Instrument::Guard<AddQueues> instrGuard;

	if (ticket.isBlocking()) {
		BlockingEntry entry(request, ticket);
		_blockingEntries.push(entry, [this]() { checkEntryQueues(); });
	} else {
		NonBlockingEntry entry(request, ticket);
		_nonBlockingEntries.push(entry, [this]() { checkEntryQueues(); });
	}
}

template <typename Lang>
void TicketManager<Lang>::addTicket(Ticket &ticket, ArrayView<request_t> &requests)
{
	Instrument::Guard<AddQueues> instrGuard;

	if (ticket.isBlocking()) {
		addTicketRequests(ticket, requests, _blockingEntries);
	} else {
		addTicketRequests(ticket, requests, _nonBlockingEntries);
	}
}

template <typename Lang>
template <typename EntryTy>
void TicketManager<Lang>::addTicketRequests(
	Ticket &ticket,
	ArrayView<request_t> &requests,
	LockFreeQueue<EntryTy> &queue
) {
	Uninitialized<EntryTy, BatchSize> entries;

	const int active = ticket.getPendingRequests();
	assert(active > 0);

	int req = 0;
	for (int added = 0; added < active; added += BatchSize) {
		const int size = std::min(BatchSize, active - added);
		int entry = 0;
		while (entry < size) {
			if (requests[req] != Interface<Lang>::REQUEST_NULL) {
				new (&entries[entry]) EntryTy(requests[req], ticket, req);
				++entry;
			}
			++req;
		}
		queue.push(entries.get(), size, [this]() { checkEntryQueues(); });
	}
}

template <typename Lang>
void TicketManager<Lang>::internalCheckEntryQueues()
{
	Instrument::Guard<TransferQueues> instrGuard;

	Uninitialized<BlockingEntry, BatchSize> blockings;
	Uninitialized<NonBlockingEntry, BatchSize> nonBlockings;

	const int numAvailable = (MaxEntries - _pending);
	int numBlk, numNonBlk;
	int total = 0;

	do {
		numBlk = std::min(numAvailable - total, BatchSize);
		numBlk = _blockingEntries.pop(blockings.get(), numBlk);
		if (numBlk > 0) {
			transferEntries(blockings.get(), numBlk);
			total += numBlk;
		}

		numNonBlk = std::min(numAvailable - total, BatchSize);
		numNonBlk = _nonBlockingEntries.pop(nonBlockings.get(), numNonBlk);
		if (numNonBlk > 0) {
			transferEntries(nonBlockings.get(), numNonBlk);
			total += numNonBlk;
		}
	} while (total < numAvailable && (numBlk > 0 || numNonBlk > 0));
}

template <typename Lang>
void TicketManager<Lang>::transferEntries(BlockingEntry *entries, int count)
{
	assert(_pending + count <= MaxEntries);

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
void TicketManager<Lang>::transferEntries(NonBlockingEntry *entries, int count)
{
	assert(_pending + count <= MaxEntries);

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

} // namespace tampi

#endif // TICKET_MANAGER_HPP
