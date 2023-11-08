/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef POLLING_PERIOD_CTRL_HPP
#define POLLING_PERIOD_CTRL_HPP

#include <cstdint>

#include "util/EnvironmentVariable.hpp"

namespace tampi {

class PollingPeriodCtrl {
	//! The period for polling tasks in microseconds
	EnvironmentVariable<uint64_t> _period;

public:
	//! \brief Create the frequency controller
	//!
	//! The controller reads the TAMPI_POLLING_PERIOD envar to determine the period in
	//! which TAMPI will check its MPI requests. The default value when the envar is
	//! not defined is 100us
	PollingPeriodCtrl() :
		_period("TAMPI_POLLING_PERIOD", 100)
	{
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
