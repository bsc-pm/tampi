/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef SLOW_START_POLLING_PERIOD_CTRL_HPP
#define SLOW_START_POLLING_PERIOD_CTRL_HPP

#include <cstdint>

#include "PollingPeriodCtrl.hpp"


namespace tampi {

//! Class implementing a Slow Start-based policy for controlling the
//! dynamic polling period
class SlowStartPollingPeriodCtrl : public DynamicPollingPeriodCtrlImpl {
	//! The number of consecutive misses before increasing period
	static constexpr int64_t Tolerance = 50;

	//! The factor used to compute the base period after a period increase
	static constexpr double Penalty = 2.0;

	//! The minimum base period after a period increase. A slow start phase
	//! cannot start from a lower value
	static constexpr double MinBase = 10.0;

	//! The minimum and maximum periods
	const double _minPeriod;
	const double _maxPeriod;

	//! The current period
	double _period;

	//! The current period decreasing factor. This factor must start at 2.0
	//! and can decrease to 1.5
	double _factor;

	//! The current number of consecutive missing
	int64_t _misses;

public:
	inline SlowStartPollingPeriodCtrl(uint64_t periodMin, uint64_t periodMax) :
		_minPeriod(periodMin), _maxPeriod(periodMax), _period(periodMax),
		_factor(2.0), _misses(0)
	{
	}

	//! Run one step of the policy after having checked the TAMPI requests
	//!
	//! \param completedUnits The number of completed requests/operations
	//! \param pendingUnits The number of pending requests/operations
	//!
	//! \returns The next polling period in microseconds
	inline uint64_t run(uint64_t completedUnits, uint64_t) override
	{
		// Tolerate some misses before increasing the period
		if (completedUnits == 0 && ++_misses < Tolerance)
		    return _period;

		// Reset the misses counter
		_misses = 0;

		if (completedUnits == 0) {
			// New miss detected. Reset the period to a higher value by applying a
			// penality. Recompute the factor depending on the new base period
			_period = std::max(std::min(_period * Penalty, _maxPeriod), MinBase);
			_factor = (1.5 * _maxPeriod + 0.5 * _period) / _maxPeriod;
		} else {
			// Decrease the current period applying the factor
			_period = std::max(_period / _factor, _minPeriod);
		}

		assert(_factor >= 1.5 && _factor <= 2.0);
		assert(_period <= _maxPeriod && _period >= _minPeriod);

		return _period;
	}
};

} // namespace tampi

#endif // SLOW_START_POLLING_PERIOD_CTRL_HPP
