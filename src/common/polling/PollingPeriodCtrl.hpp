/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef POLLING_PERIOD_CTRL_HPP
#define POLLING_PERIOD_CTRL_HPP

#include <cassert>
#include <cstdint>
#include <string>

#include "util/EnvironmentVariable.hpp"


namespace tampi {

//! Virtual class for dynamic polling period controllers. Policies should
//! inherit from this class
struct DynamicPollingPeriodCtrlImpl {
	virtual ~DynamicPollingPeriodCtrlImpl()
	{
	}

	//! Run one step of the policy after having checked the TAMPI requests
	//!
	//! \param completedUnits The number of completed requests/operations
	//! \param pendingUnits The number of pending requests/operations
	//!
	//! \returns The next polling period in microseconds
	virtual uint64_t run(uint64_t completedUnits, uint64_t pendingUnits) = 0;
};

//! Class that controls the polling period depending on the policies specified
//! by the user
class PollingPeriodCtrl {
	//! The default period and policy
	constexpr static uint64_t DefaultPeriodUs = 100;
	constexpr static const char *DefaultPolicy = "slowstart";

	//! The envar specifying the polling period
	EnvironmentVariable<std::string> _envar;

	//! The minimum period for polling tasks
	uint64_t _minPeriod;

	//! The maximum period for polling tasks
	uint64_t _maxPeriod;

	//! Whether the polling period is dynamic or static
	bool _dynamic;

	//! Policy for dynamic polling
	std::string _policy;

	//! Dynamic period controller
	DynamicPollingPeriodCtrlImpl *_dynamicCtrl;

public:
	//! \brief Create the period controller
	//!
	//! The controller reads the TAMPI_POLLING_PERIOD envar to determine the period
	//! in which TAMPI will check its MPI requests. If the envar is not defined,
	//! the period is 100us as a default value
	//!
	//! \param envar The envar specifying the polling period
	PollingPeriodCtrl(const std::string &envar);

	~PollingPeriodCtrl();

	//! \brief Run the polling policy and retrieve the next polling period
	//!
	//! \param completed The number of completed requests/operations
	//! \param pending The number of pending requests/operations
	//!
	//! \returns The next polling period in microseconds
	inline uint64_t getPeriod(size_t completed, size_t pending)
	{
		if (!_dynamic)
			return _maxPeriod;

		assert(_dynamicCtrl != nullptr);
		return _dynamicCtrl->run(completed, pending);
	}
};

} // namespace tampi

#endif // POLLING_PERIOD_CTRL_HPP
