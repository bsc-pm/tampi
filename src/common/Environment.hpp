/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <mpi.h>

#include "TAMPI_Decl.h"

#include <atomic>
#include <cstdint>
#include <mutex>

#include "Interface.hpp"
#include "Polling.hpp"
#include "TaskContext.hpp"
#include "TaskingModel.hpp"
#include "TicketManager.hpp"
#include "instrument/Instrument.hpp"
#include "util/ErrorHandler.hpp"


namespace tampi {

//! Class that represents the environment and controls
//! which TAMPI modes are enabled
class Environment {
private:
	struct State {
		//! Indicate whether the library is preinitialized. This means
		//! that the MPI environment was correctly setup
		bool preinitialized;

		//! Indicate whether the library is initialized
		bool initialized;

		//! Indicate whether the thread level provided by MPI is multithreaded
		int nativeThreadLevel;

		//! Indicate whether the blocking and non-blocking modes are enabled
		std::atomic<bool> blockingMode;
		std::atomic<bool> nonBlockingMode;

		//! Mutex protecting the state
		std::mutex mutex;

		// Construct the initial state
		State() :
			preinitialized(false),
			initialized(false),
			nativeThreadLevel(MPI_THREAD_SINGLE),
			blockingMode(false),
			nonBlockingMode(false),
			mutex()
		{
		}
	};

	//! The structure holding the current state
	static State _state;

public:
	Environment() = delete;
	Environment(const Environment &) = delete;
	const Environment& operator= (const Environment &) = delete;

	//! \brief Check whether the blocking TAMPI mode is enabled
	static inline bool isBlockingEnabled()
	{
		return _state.blockingMode.load(std::memory_order_acquire);
	}

	//! \brief Check whether the non-blocking TAMPI mode is enabled
	static inline bool isNonBlockingEnabled()
	{
		return _state.nonBlockingMode.load(std::memory_order_acquire);
	}

	//! \brief Get the current value of a property
	//!
	//! \param property The property identifier to retrieve
	//! \param value Pointer to where the value should be saved
	//!
	//! \return Zero on success, and error otherwise
	static inline int getProperty(int property, int *value)
	{
		std::lock_guard<std::mutex> lock(_state.mutex);

		switch (property) {
			case TAMPI_PROPERTY_BLOCKING_MODE:
				*value = _state.blockingMode;
				break;
			case TAMPI_PROPERTY_NONBLOCKING_MODE:
				*value = _state.nonBlockingMode;
				break;
			default:
				return 1;
		}
		return 0;
	}

	//! \brief Set the current value of a property
	//!
	//! Notice that some properties may not be modifiable at all
	//! or they may have specfic conditions when they can be
	//! modifiable
	//!
	//! \param property The property identifier to modify
	//! \param value The new value
	//!
	//! \return Zero on success, and error otherwise
	static inline int setProperty(int property, int)
	{
		std::lock_guard<std::mutex> lock(_state.mutex);

		switch (property) {
			// Setting these infos is never valid
			case TAMPI_PROPERTY_BLOCKING_MODE:
			case TAMPI_PROPERTY_NONBLOCKING_MODE:
			default:
				return 1;
		}
		return 0;
	}

	//! \brief Preinitialize the MPI-related environment
	//!
	//! This function should be called after a successful call to the original
	//! MPI_Init or MPI_Init_thread. This function does not initialize the
	//! task-awareness; a subsequent call to Environment::initialize is needed
	//!
	//! \param provided The thread level provided by the MPI
	static void preinitialize(int provided)
	{
		std::lock_guard<std::mutex> lock(_state.mutex);

		assert(!_state.preinitialized);
		assert(!_state.initialized);

#if !defined(DISABLE_C_LANG)
		Interface<C>::initialize();
#endif

#if !defined(DISABLE_FORTRAN_LANG)
		Interface<Fortran>::initialize();
#endif

		// Save the thread level provided by MPI
		_state.nativeThreadLevel = provided;

		// Mark the library as preinitialized; the MPI environment is ready
		_state.preinitialized = true;
	}

	//! \brief Initialize the task-awareness library
	//!
	//! \param required the required thread level to TAMPI
	//! \param provided the provided thread level by TAMPI
	static void initialize(int required, int *provided)
	{
		bool enableBlocking = shouldEnableBlocking(required);
		bool enableNonBlocking = shouldEnableNonBlocking(required);

		std::lock_guard<std::mutex> lock(_state.mutex);

		// Perform several checks to decide if we must initialize
		bool initialize = shouldInitialize();
		if (!initialize)
			return;

		// Task-aware modes must be disabled if the MPI library does not work
		// with multithreading support
		if (_state.nativeThreadLevel != MPI_THREAD_MULTIPLE) {
			enableBlocking = false;
			enableNonBlocking = false;
		}

		int rank = InterfaceAny::rank;
		int nranks = InterfaceAny::nranks;

		// Find the tasking runtime symbols
		TaskingModel::initialize(enableBlocking, enableNonBlocking);

		// Initialize the instrumentation
		Instrument::initialize(rank, nranks);

		// Launch a polling task if required
		if (enableBlocking || enableNonBlocking)
			Polling::initialize();

		// Mark the library as initialized
		_state.blockingMode.store(enableBlocking, std::memory_order_release);
		_state.nonBlockingMode.store(enableNonBlocking, std::memory_order_release);
		_state.initialized = true;

		// Return the provided threading level
		if (enableBlocking)
			*provided = MPI_TASK_MULTIPLE;
		else
			*provided = _state.nativeThreadLevel;
	}

	//! \brief Finalize the task-awareness library
	static void finalize()
	{
		std::lock_guard<std::mutex> lock(_state.mutex);

		// Perform several checks to decide if we must finalize
		bool finalize = shouldFinalize();
		if (!finalize)
			return;

		// Finalize the instrumentation
		Instrument::finalize();

		// Finalize the polling task if it was running
		if (_state.blockingMode || _state.nonBlockingMode)
			Polling::finalize();

		// Disable both modes
		_state.blockingMode.store(false, std::memory_order_acq_rel);
		_state.nonBlockingMode.store(false, std::memory_order_acq_rel);

		// Mark the library as finalized
		_state.initialized = false;
	}

private:
	//! \brief Indicate if should enable blocking mode given a thread level
	static bool shouldEnableBlocking([[maybe_unused]] int level)
	{
#ifndef DISABLE_BLOCKING_MODE
		return (level == MPI_TASK_MULTIPLE);
#else
		return false;
#endif
	}

	//! \brief Indicate if should enable non-blocking mode given a thread level
	static bool shouldEnableNonBlocking([[maybe_unused]] int level)
	{
#ifndef DISABLE_NONBLOCKING_MODE
		return (level == MPI_TASK_MULTIPLE || level == MPI_THREAD_MULTIPLE);
#else
		return false;
#endif
	}

	//! \brief Indicate whether should initialize
	static bool shouldInitialize()
	{
		// The library should be already preinitialized
		if (!_state.preinitialized)
			ErrorHandler::fail("Intializing TAMPI before MPI is invalid");

		return true;
	}

	//! \brief Indicate whether should finalize
	static bool shouldFinalize()
	{
		// The library must be initialized before finalizing
		if (!_state.initialized)
			ErrorHandler::fail("Finalizing TAMPI before initializing is invalid");

		return true;
	}
};

} // namespace tampi

#endif // ENVIRONMENT_HPP
