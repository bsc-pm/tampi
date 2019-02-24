/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef TICKET_MANAGER_HPP
#define TICKET_MANAGER_HPP

#include "util/array_view.hpp"
#include "util/SpinLock.hpp"
#include "util/Utils.hpp"

#include "MultiAdderQueue.hpp"
#include "Symbols.hpp"
#include "Ticket.hpp"
#include "TicketManagerInternals.hpp"

#include <algorithm>
#include <cstring>
#include <functional>
#include <mutex>
#include <vector>

#include <mpi.h>

template <typename Lang>
class TicketManager {
private:
	static const int NENTRIES = 32*1000;
	static const int NRPG = 64;
	
	typedef typename Lang::request_t request_t;
	typedef typename Lang::status_t status_t;
	typedef typename Lang::status_ptr_t status_ptr_t;
	typedef ::Ticket<Lang> Ticket;
	typedef std::function<void()> ProgressFunction;
	typedef util::SpinLock<> SpinLock;
	
	struct BlockingEntry {
		request_t _request;
		Ticket *_ticket;
		int _position;
		
		inline BlockingEntry()
		{}
		
		inline BlockingEntry(const request_t &request, Ticket &ticket, int position = 0)
			: _request(request), _ticket(&ticket), _position(position)
		{}
	};
	
	struct NonBlockingEntry {
		request_t _request;
		status_ptr_t _status;
		TaskContext _taskContext;
		int _position;
		
		inline NonBlockingEntry()
		{}
		
		inline NonBlockingEntry(const request_t &request, status_ptr_t status, const TaskContext &taskContext, int position = 0)
			: _request(request), _status(status), _taskContext(taskContext), _position(position)
		{}
	};
	
	int _pending;
	TicketManagerInternals<Lang, NENTRIES> _arrays;
	
	MultiAdderQueue<BlockingEntry> _blockingEntries;
	MultiAdderQueue<NonBlockingEntry> _nonBlockingEntries;
	
	ProgressFunction _checkEntriesFunc;
	
	int *_indices;
	mutable SpinLock _mutex;
	
public:
	TicketManager()
		: _pending(0),
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
	
	inline void completeRequest(unsigned int position, const status_t &status)
	{
		int localPosition = _arrays.getLocalPositionInTicket(position);
		Ticket &ticket = _arrays.getAssociatedTicket(position);
		ticket.requestCompleted(status, localPosition);
	}
	
	void checkRequests()
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
	
	void addBlockingRequest(request_t &request, Ticket &ticket);
	
	void addBlockingRequests(util::array_view<request_t> &requests, Ticket &ticket);
	
	void addNonBlockingRequest(request_t &request, status_ptr_t status);
	
	void addNonBlockingRequests(util::array_view<request_t> &requests, status_ptr_t statuses);
	
private:
	bool internalCheckRequests();
	
	inline void checkEntryQueues()
	{
		std::unique_lock<SpinLock> guard(_mutex, std::try_to_lock);
		if (guard.owns_lock()) {
			internalCheckEntryQueues();
		}
	}
	
	void internalCheckEntryQueues();
	
	void transferEntries(BlockingEntry entries[], int count);
	
	void transferEntries(NonBlockingEntry entries[], int count);
};

template <>
inline bool TicketManager<C>::internalCheckRequests()
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
inline bool TicketManager<Fortran>::internalCheckRequests()
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

template<typename Lang>
inline void TicketManager<Lang>::addBlockingRequest(request_t &request, Ticket &ticket)
{
	assert(request != Lang::REQUEST_NULL);
	ticket.addPendingRequest();
	
	BlockingEntry entry(request, ticket);
	_blockingEntries.add(entry, _checkEntriesFunc);
}

template<typename Lang>
inline void TicketManager<Lang>::addNonBlockingRequest(request_t &request, status_ptr_t status)
{
	assert(request != Lang::REQUEST_NULL);
	TaskContext taskContext(false);
	taskContext.bindEvents(1);
	
	NonBlockingEntry entry(request, status, taskContext);
	_nonBlockingEntries.add(entry, _checkEntriesFunc);
}

template<typename Lang>
inline void TicketManager<Lang>::addBlockingRequests(util::array_view<request_t> &requests, Ticket &ticket)
{
	BlockingEntry entries[NRPG];
	
	const int active = util::getActiveRequestCount<Lang>(requests);
	assert(active > 0);
	ticket.addPendingRequest(active);
	
	int req = 0;
	for (int added = 0; added < active; added += NRPG) {
		const int size = std::min(NRPG, active - added);
		int entry = 0;
		while (entry < size) {
			if (requests[req] != Lang::REQUEST_NULL) {
				new (&entries[entry]) BlockingEntry(requests[req], ticket, req);
				++entry;
			}
			++req;
		}
		_blockingEntries.add(entries, size, _checkEntriesFunc);
	}
}

template<typename Lang>
inline void TicketManager<Lang>::addNonBlockingRequests(util::array_view<request_t> &requests, status_ptr_t statuses)
{
	NonBlockingEntry entries[NRPG];
	
	const int active = util::getActiveRequestCount<Lang>(requests);
	assert(active > 0);
	
	TaskContext taskContext(false);
	taskContext.bindEvents(active);
	
	int req = 0;
	for (int added = 0; added < active; added += NRPG) {
		const int size = std::min(NRPG, active - added);
		int entry = 0;
		while (entry < size) {
			if (requests[req] != Lang::REQUEST_NULL) {
				new (&entries[entry]) NonBlockingEntry(requests[req], statuses, taskContext, req);
				++entry;
			}
			++req;
		}
		_nonBlockingEntries.add(entries, size, _checkEntriesFunc);
	}
}

template<typename Lang>
inline void TicketManager<Lang>::internalCheckEntryQueues()
{
	const int numAvailable = (NENTRIES - _pending);
	BlockingEntry blockings[NRPG];
	NonBlockingEntry nonBlockings[NRPG];
	int numBlk, numNonBlk;
	int total = 0;
	
	do {
		numBlk = std::min(numAvailable - total, NRPG);
		numBlk = _blockingEntries.retrieve(blockings, numBlk);
		transferEntries(blockings, numBlk);
		total += numBlk;
		
		numNonBlk = std::min(numAvailable - total, NRPG);
		numNonBlk = _nonBlockingEntries.retrieve(nonBlockings, numNonBlk);
		transferEntries(nonBlockings, numNonBlk);
		total += numNonBlk;
	} while (total < numAvailable && (numBlk > 0 || numNonBlk > 0));
}

template<typename Lang>
inline void TicketManager<Lang>::transferEntries(BlockingEntry entries[], int count)
{
	assert(_pending + count <= NENTRIES);
	
	const int pending = _pending;
	for (int r = 0; r < count; ++r) {
		BlockingEntry &entry = entries[r];
		Ticket &ticket = *(entry._ticket);
		
		_arrays.associateRequest(pending + r,
				entry._request, ticket,
				entry._position);
	}
	_pending += count;
}

template<typename Lang>
inline void TicketManager<Lang>::transferEntries(NonBlockingEntry entries[], int count)
{
	assert(_pending + count <= NENTRIES);
	
	const int pending = _pending;
	for (int r = 0; r < count; ++r) {
		NonBlockingEntry &entry = entries[r];
		Ticket &ticket = _arrays.allocateTicket(pending + r, entry._status, entry._taskContext);
		
		_arrays.associateRequest(pending + r, entry._request, ticket, entry._position);
	}
	_pending += count;
}

template<typename Lang>
const int TicketManager<Lang>::NRPG;

#endif // TICKET_MANAGER_HPP
