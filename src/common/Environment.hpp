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

		//! Indicate whether TAMPI should initialize/finalize when MPI does. By
		//! default is enabled
		bool autoInitialize;

		//! Indicate whether the thread level provided by MPI is multithreaded
		int nativeThreadLevel;

		//! Indicate whether the thread has task-awareness enabled. By default
		//! is enabled
		static thread_local bool threadTaskAwareness;

		//! Indicate whether the blocking and non-blocking modes are enabled
		std::atomic<bool> blockingMode;
		std::atomic<bool> nonBlockingMode;

		//! Mutex protecting the state
		std::mutex mutex;

		// Construct the initial state
		State() :
			preinitialized(false),
			initialized(false),
			autoInitialize(true),
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
	static bool isBlockingEnabled()
	{
		return _state.blockingMode.load(std::memory_order_acquire);
	}

	//! \brief Check whether the non-blocking TAMPI mode is enabled
	static bool isNonBlockingEnabled()
	{
		return _state.nonBlockingMode.load(std::memory_order_acquire);
	}

	//! \brief Check whether the auto initialization is enabled
	static bool isAutoInitializeEnabled()
	{
		int enabled;
		getProperty(TAMPI_PROPERTY_AUTO_INIT, &enabled);
		return enabled;
	}

	//! \brief Check whether the blocking TAMPI mode is enabled
	//!
	//! This function checks whether the blocking mode is enabled globally
	//! and whether the thread has the mode enabled
	static bool isBlockingEnabledForCurrentThread()
	{
		return (_state.threadTaskAwareness && isBlockingEnabled());
	}

	//! \brief Get the current value of a property
	//!
	//! \param property The property identifier to retrieve
	//! \param value Pointer to where the value should be saved
	//!
	//! \return Zero on success, and error otherwise
	static int getProperty(int property, int *value)
	{
		std::lock_guard<std::mutex> lock(_state.mutex);

		switch (property) {
			case TAMPI_PROPERTY_BLOCKING_MODE:
				*value = _state.blockingMode;
				break;
			case TAMPI_PROPERTY_NONBLOCKING_MODE:
				*value = _state.nonBlockingMode;
				break;
			case TAMPI_PROPERTY_THREAD_TASKAWARE:
				*value = _state.threadTaskAwareness;
				break;
			case TAMPI_PROPERTY_AUTO_INIT:
				*value = _state.autoInitialize;
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
	static int setProperty(int property, int value)
	{
		std::lock_guard<std::mutex> lock(_state.mutex);

		switch (property) {
			case TAMPI_PROPERTY_BLOCKING_MODE:
			case TAMPI_PROPERTY_NONBLOCKING_MODE:
				// Setting these infos is never valid
				return 1;
			case TAMPI_PROPERTY_THREAD_TASKAWARE:
				_state.threadTaskAwareness = value;
				break;
			case TAMPI_PROPERTY_AUTO_INIT:
				// Not valid to set this info after MPI_Init/MPI_Init_thread
				if (_state.preinitialized)
					return 1;
				_state.autoInitialize = value;
				break;
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
	//! \param automatic Whether it is an automatic initilization attempt
	static void initialize(int required, int *provided, bool automatic)
	{
		bool enableBlocking = shouldEnableBlocking(required);
		bool enableNonBlocking = shouldEnableNonBlocking(required);

		std::lock_guard<std::mutex> lock(_state.mutex);

		// Perform several checks to decide if we must initialize
		bool initialize = shouldInitialize(automatic);
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
	//!
	//! \param automatic Whether it is an atomatic finalization attempt
	static void finalize(bool automatic)
	{
		std::lock_guard<std::mutex> lock(_state.mutex);

		// Perform several checks to decide if we must finalize
		bool finalize = shouldFinalize(automatic);
		if (!finalize)
			return;

		// Finalize the instrumentation
		Instrument::finalize();

		// Finalize the polling task if it was running
		if (_state.blockingMode || _state.nonBlockingMode)
			Polling::finalize();

		// Disable both modes
		_state.blockingMode.store(false);
		_state.nonBlockingMode.store(false);

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
	//!
	//! \param automatic Whether the initialization attempt is automatic
	//!
	//! \return Whether should initialize
	static bool shouldInitialize(bool automatic)
	{
		// The library should be already preinitialized
		if (!_state.preinitialized)
			ErrorHandler::fail("Intializing TAMPI before MPI is invalid");

		// Cannot explicitly initialize if TAMPI_PROPERTY_AUTO_INIT is enabled
		if (!automatic && _state.autoInitialize)
			ErrorHandler::fail("Explicitly initializing TAMPI is invalid by default");

		// Do not auto initialize when auto initialization is disabled
		return (!automatic || _state.autoInitialize);
	}

	//! \brief Indicate whether should finalize
	//!
	//! \param automatic Whether the finalization attempt is automatic
	static bool shouldFinalize(bool automatic)
	{
		// Auto finalization is disabled; the library should be finalized
		if (automatic && !_state.autoInitialize) {
			if (_state.initialized) {
				// Library should be already explicitly finalized but it is not. Print
				// a warning and finalize the library
				ErrorHandler::warn("TAMPI_Finalize must be called before MPI_Finalize");
				return true;
			}
			// Otherwise, do not finalize
			return false;
		}

		// Cannot explicitly finalize if auto initialize is enabled
		if (!automatic && _state.autoInitialize)
			ErrorHandler::fail("Explicitly initializing TAMPI is invalid by default");

		// The library must be initialized before finalizing
		if (!_state.initialized)
			ErrorHandler::fail("Finalizing TAMPI before initializing is invalid");

		return true;
	}
};

} // namespace tampi

#endif // ENVIRONMENT_HPP
