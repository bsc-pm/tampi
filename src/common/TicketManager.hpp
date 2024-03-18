/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef TICKET_MANAGER_HPP
#define TICKET_MANAGER_HPP

#include <mpi.h>

#include <cstdint>
#include <algorithm>
#include <mutex>

#include "CompletionManager.hpp"
#include "Interface.hpp"
#include "Operation.hpp"
#include "TaskingModel.hpp"
#include "Ticket.hpp"
#include "TicketManagerCapacityCtrl.hpp"
#include "TicketManagerInternals.hpp"
#include "instrument/Instrument.hpp"
#include "util/ArrayView.hpp"
#include "util/BoostLockFreeQueue.hpp"
#include "util/MultiLockFreeQueue.hpp"
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
	//! Maximum number of requests to be inserted at once
	static constexpr int BatchSize = 64;

	typedef typename Types<Lang>::request_t request_t;
	typedef typename Types<Lang>::status_t status_t;
	typedef typename Types<Lang>::status_ptr_t status_ptr_t;

	typedef tampi::Ticket<Lang> Ticket;

	typedef tampi::Operation<Lang> Operation;
	typedef tampi::CollOperation<Lang> CollOperation;

	template <typename T>
	struct EntryBase {
		T _operation;
		Ticket _ticket;

		EntryBase(const T &operation, const Ticket &ticket) :
			_operation(operation), _ticket(ticket)
		{
		}

		EntryBase()
		{
		}
	};

	typedef EntryBase<Operation> P2PEntry;
	typedef EntryBase<CollOperation> CollEntry;

	template <typename T>
	using P2PEntryMultiQueue = MultiLockFreeQueue<T, MultiQueuePopPolicy::CyclicRoundRobin>;
	template <typename T>
	using CollEntryQueue = BoostLockFreeQueue<T>;

	//! The approaches for request testing
	enum class TestingApproach {
		Test = 0,
		TestSome,
		TestAny,
		None
	};

	//! Indicates how the MPI requests should tested in general
	TestingApproach _generalTesting;

	//! Indicates whether and how the MPI requests should tested immediately
	TestingApproach _immediateTesting;

	//! The controller of how many in-flight requests we allow concurrently
	TicketManagerCapacityCtrl _capacityCtrl;

	//! Number of current in-flight requests
	int _pending;

	//! General arrays storing the tickets, requests and statuses
	TicketManagerInternals<Lang, TicketManagerCapacityCtrl::max()> _arrays;

	//! Pre-queues for point-to-point operations
	P2PEntryMultiQueue<P2PEntry> _p2pEntries;

	//! Pre-queues for collective operations
	CollEntryQueue<CollEntry> _collEntries;

	//! Spinlock for consuming requests from pre-queues and checking in-flight
	//! requests from the general array
	mutable SpinLock _mutex;

public:
	TicketManager() :
		_generalTesting(parseTestingOption("TAMPI_REQUESTS_TESTING", TestingApproach::TestSome)),
		_immediateTesting(parseTestingOption("TAMPI_REQUESTS_IMMEDIATE_TESTING", _generalTesting)),
		_capacityCtrl(), _pending(0), _arrays(), _p2pEntries(),
		_collEntries(), _mutex()
	{
		if (_generalTesting == TestingApproach::None)
			ErrorHandler::fail("Invalid approach for general request testing");
	}

	TicketManager(const TicketManager &) = delete;
	const TicketManager& operator= (const TicketManager &) = delete;

	//! \brief Get the ticket manager for a specific language
	//!
	//! \returns A reference to the ticket manager
	static TicketManager &get()
	{
		static TicketManager _ticketManager;
		return _ticketManager;
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

		size_t totalCompleted = 0;
		size_t inserted, completed;

		do {
			inserted = completed = 0;

			if (_pending < _capacityCtrl.get())
				inserted = internalCheckEntryQueues(BatchSize*2);

			if (_pending)
				completed = internalCheckRequests();
			totalCompleted += completed;
		} while (completed > 0 || inserted > 0);

		// Evaluate what should be the current capacity
		_capacityCtrl.evaluate(_pending, totalCompleted);

		pending += _pending;

		return totalCompleted;
	}

	//! \brief Add a ticket and its operation to the pre-queues
	//!
	//! \param ticket The ticket which the request belongs to
	//! \param operation The operation associated to the ticket
	template <typename T>
	void addTicket(Ticket &ticket, T &operation)
	{
		if constexpr (std::is_same_v<T, Operation>) {
			P2PEntry entry(operation, ticket);
			_p2pEntries.push(entry);
		} else {
			CollEntry entry(operation, ticket);
			_collEntries.push(entry);
		}
	}

private:
	//! \brief Check the in-flight requests
	//!
	//! \returns The number of requests completed
	int internalCheckRequests();

	static int internalTestRequests(TestingApproach approach, int count, request_t *requests, int *indices, status_ptr_t statuses)
	{
		Instrument::Guard<TestSomeRequests> instrGuard;

		int completed = 0;
		if (approach == TestingApproach::TestSome) {
			completed = Interface<Lang>::testsome(count, requests, indices, statuses);
		} else if (approach == TestingApproach::TestAny) {
			bool cont = true;
			int index, processed = 0;
			while (cont && processed < count) {
				cont = Interface<Lang>::testany(count - processed, &requests[processed], &index, statuses + completed);
				if (cont) {
					index = index + processed;
					processed = index + 1;
					indices[completed++] = index;
				}
			}
		} else if (approach == TestingApproach::Test) {
			for (int r = 0; r < count; ++r) {
				if (Interface<Lang>::test(requests[r], statuses + completed))
					indices[completed++] = r;
			}
		}
		return completed;
	}

	//! \brief Internal function to check and transfer requests from pre-queues
	//!
	//! This function assumes the lock is already acquired
	int internalCheckEntryQueues(int maxEntries);

	//! \brief Transfers operations to the general array
	//!
	//! \param entries The array of entries containing the operations to transfer
	//! \param count The number of operations to add
	template <typename OperationTy>
	void transferEntries(EntryBase<OperationTy> *entries, int count);

	//! \brief Parse request testing option
	static TestingApproach parseTestingOption(
		const std::string &name, TestingApproach defaultValue
	) {
		EnvironmentVariable<std::string> value(name);

		if (!value.isPresent())
			return defaultValue;

		if (value.get() == "testsome")
			return TestingApproach::TestSome;
		if (value.get() == "testany")
			return TestingApproach::TestAny;
		if (value.get() == "test")
			return TestingApproach::Test;
		if (value.get() == "none")
			return TestingApproach::None;

		ErrorHandler::fail(name, " has invalid value");

		return TestingApproach::None;
	}
};

template <typename Lang>
inline int TicketManager<Lang>::internalCheckRequests()
{
	assert(_pending > 0);

	Instrument::Guard<CheckGlobalRequestArray> instrGuard;

	int *indices = _arrays.getIndices();
	assert(indices != nullptr);

	Uninitialized<TaskContext, BatchSize> contexts;

	bool useCompletionManager = CompletionManager::isEnabled();

	int checked = 0;
	int completed = 0;
	do {
		int count = std::min(_pending - checked, BatchSize);

		int batchCompleted = internalTestRequests(_generalTesting,
				count, _arrays.getRequests() + checked,
				indices + completed, _arrays.getStatuses());

		for (int c = 0; c < batchCompleted; ++c) {
			// Correct indices to consider batch offset
			indices[completed + c] += checked;

			int index = indices[completed + c];
			int local = _arrays.getLocalPositionInTicket(index);
			Ticket &ticket = _arrays.getAssociatedTicket(index);
			if (!ticket.ignoreStatus())
				ticket.storeStatus(_arrays.getStatus(c), local);

			if (useCompletionManager) {
				contexts[c] = ticket.getTaskContext();
			} else {
				// Complete requests
				Instrument::enter<CompletedRequest>();
				ticket.complete();
				Instrument::exit<CompletedRequest>();
			}
		}

		if (useCompletionManager)
			CompletionManager::transfer((TaskContext *) contexts, batchCompleted);

		checked += count;
		completed += batchCompleted;
	} while (checked < _pending);

	// Perform smart replacement to keep the arrays compact
	int replacement = _pending - 1;
	int reverse = completed - 1;
	for (int c = 0; c < completed; ++c) {
		const int current = indices[c];

		bool replace = false;
		while (replacement > current) {
			if (replacement != indices[reverse]) {
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

	return completed;
}

template <typename Lang>
inline int TicketManager<Lang>::internalCheckEntryQueues(int maxEntries)
{
	Instrument::Guard<TransferQueues> instrGuard;

	Uninitialized<P2PEntry, BatchSize> tmpP2PEntries;
	Uninitialized<CollEntry, BatchSize> tmpCollEntries;

	const int navailable = std::min(_capacityCtrl.get() - _pending, maxEntries);
	int np2p, ncoll;
	int ntotal = 0;

	do {
		np2p = std::min(navailable - ntotal, BatchSize);
		np2p = _p2pEntries.pop(tmpP2PEntries, np2p);
		if (np2p > 0) {
			transferEntries(tmpP2PEntries.get(), np2p);
			ntotal += np2p;
		}

		ncoll = std::min(navailable - ntotal, BatchSize);
		ncoll = _collEntries.pop(tmpCollEntries, ncoll);
		if (ncoll > 0) {
			transferEntries(tmpCollEntries.get(), ncoll);
			ntotal += ncoll;
		}
	} while (ntotal < navailable && (np2p > 0 || ncoll > 0));

	return ntotal;
}

template <typename Lang>
template <typename OperationTy>
inline void TicketManager<Lang>::transferEntries(EntryBase<OperationTy> *entries, int count)
{
	assert(count <= BatchSize);
	assert(_pending + count <= _capacityCtrl.get());

	request_t requests[BatchSize];
	status_t statuses[BatchSize];

	bool useCompletionManager = CompletionManager::isEnabled();

	int complentries[BatchSize];
	int req2entry[BatchSize];
	int testcompl2req[BatchSize];

	Uninitialized<TaskContext, BatchSize> contexts;

	int ncompl = 0;
	int nreqs = 0;
	int ntestcompl = 0;

	for (int e = 0; e < count; ++e) {
		Instrument::enter<IssueNonBlockingOp>();

		// Issue the non-blocking operation
		requests[nreqs] = entries[e]._operation.issue();
		if (requests[nreqs] != Interface<Lang>::REQUEST_NULL) {
			req2entry[nreqs++] = e;
		} else {
			complentries[ncompl++] = e;
		}

		Instrument::exit<IssueNonBlockingOp>();
	}

	if (nreqs)
		ntestcompl = internalTestRequests(_immediateTesting, nreqs,
				requests, testcompl2req, (status_ptr_t) statuses);

	for (int c = 0; c < ntestcompl; ++c) {
		int req = testcompl2req[c];
		int entry = req2entry[req];
		Ticket &ticket = entries[entry]._ticket;

		if (!ticket.ignoreStatus())
			ticket.storeStatus(statuses[c], 0);

		complentries[ncompl++] = entry;
		req2entry[req] = -1;
	}

	for (int c = 0; c < ncompl; ++c) {
		int entry = complentries[c];
		Ticket &ticket = entries[entry]._ticket;

		if (useCompletionManager) {
			contexts[c] = ticket.getTaskContext();
		} else {
			Instrument::enter<CompletedRequest>();
			ticket.complete();
			Instrument::exit<CompletedRequest>();
		}
	}

	if (useCompletionManager && ncompl > 0)
		CompletionManager::transfer((TaskContext *) contexts, ncompl);

	for (int r = 0; r < nreqs; ++r) {
		int entry = req2entry[r];
		if (entry < 0)
			continue;

		// Allocate a copy of the ticket and associate it with the request
		Ticket &ticket = _arrays.allocateTicket(_pending, entries[entry]._ticket);
		_arrays.associateRequest(_pending, requests[r], ticket, 0);
		++_pending;
	}
}

} // namespace tampi

#endif // TICKET_MANAGER_HPP
