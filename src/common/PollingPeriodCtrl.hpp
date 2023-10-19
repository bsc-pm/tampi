/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef POLLING_PERIOD_CTRL_HPP
#define POLLING_PERIOD_CTRL_HPP

#include <cstdint>

#include "util/ErrorHandler.hpp"
#include "util/EnvironmentVariable.hpp"

namespace tampi {

class PollingPeriodCtrl {
	//! The period for polling tasks in microseconds
	uint64_t _period;

public:
	//! \brief Create the frequency controller
	//!
	//! The controller reads the TAMPI_POLLING_PERIOD envar to determine the period in
	//! which TAMPI will check its MPI requests. If the envar is not defined, the period
	//! is 100us as a default value. Notice that TAMPI_POLLING_FREQUENCY is deprecated
	PollingPeriodCtrl() :
		_period(100)
	{
		EnvironmentVariable<uint64_t> pollingPeriod("TAMPI_POLLING_PERIOD");
		EnvironmentVariable<uint64_t> pollingFrequency("TAMPI_POLLING_FREQUENCY");

		if (pollingFrequency.isPresent())
			ErrorHandler::warn("TAMPI_POLLING_FREQUENCY is deprecated;"
				" use TAMPI_POLLING_PERIOD instead");

		// The currently valid envar has precendence over the deprecated
		if (pollingPeriod.isPresent())
			_period = pollingPeriod.get();
		else if (pollingFrequency.isPresent())
			_period = pollingFrequency.get();
	}

	//! \brief Retrieve the polling period in microseconds
	//!
	//! \params completed The number of completed requests
	//! \params pending The number of pending requests
	uint64_t getPeriod(size_t, size_t)
	{
		return _period;
	}
};

} // namespace tampi

#endif // POLLING_PERIOD_CTRL_HPP
