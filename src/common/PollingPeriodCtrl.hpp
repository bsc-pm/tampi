/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef POLLING_PERIOD_CTRL_HPP
#define POLLING_PERIOD_CTRL_HPP

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include "TaskingModel.hpp"
#include "TicketManager.hpp"
#include "util/ErrorHandler.hpp"
#include "util/EnvironmentVariable.hpp"


namespace tampi {

class RollingWindow {
	//! The rolling window has 20 positions. When 60% or more of the
	//! positions are hit, the decision is to decrease the polling
	//! period (speed up). When 20% or less of the positions are hit,
	//! the decision is to increase the polling period (speed down).
	//! Otherwise, the polling period is maintained
	static constexpr uint64_t WINDOW_SIZE = 20;
	static constexpr uint64_t DECREASE_BOUNDARY = 12;
	static constexpr uint64_t INCREASE_BOUNDARY = 4;

	//! The rolling window
	std::vector<bool> _window;

	//! The number of current samples and number of hits
	uint64_t _nsamples;
	uint64_t _nhits;

	//! \brief Retire the oldest position if necessary
	//!
	//! \returns whether there was a retire
	inline bool retire()
	{
		// No need to retire any position
		if (_nsamples < WINDOW_SIZE)
			return false;

		// Update the number of hits depending on the retired position
		if (_window[_nsamples % WINDOW_SIZE])
			--_nhits;

		return true;
	}

public:
	//! Enumeration of decisions
	enum Decision {
		MAINTAIN = 0,
		INCREASE,
		DECREASE
	};

	inline RollingWindow() :
		_window(WINDOW_SIZE, false), _nsamples(0), _nhits(0)
	{
	}

	//! \brief Run the rolling window with a new hit or miss
	//!
	//! \params hit Whether it is a new hit or miss
	//!
	//! \returns the decision
	inline Decision run(bool hit)
	{
		// Retire the oldest position if needed
		bool retired = retire();

		// Update the window and number of hits
		_window[_nsamples % WINDOW_SIZE] = hit;
		_nhits += hit;

		// Increase the number of samples if necessary
		if (!retired)
			++_nsamples;

		// Decrease the period and clean the window if necessary. If it
		// already achieve the decrease boundary, dicrease directly
		if (_nhits >= DECREASE_BOUNDARY) {
			reset();
			return DECREASE;
		}

		// Increase the period and clean the window if necessary. Wait
		// until we have the total number of samples
		if (_nsamples == WINDOW_SIZE && _nhits <= INCREASE_BOUNDARY) {
			reset();
			return INCREASE;
		}

		// Otherwise, keep the same period
		return MAINTAIN;
	}

	//! \brief Reset and clean the rolling window
	inline void reset()
	{
		_nsamples = 0;
		_nhits = 0;
	}
};

class PollingPeriodCtrl {
	static constexpr double INCREASE_STEP = 20;

	//! The minimum period for polling tasks
	uint64_t _periodMin;

	//! The maximum period for polling tasks
	uint64_t _periodMax;

	//! Whether the polling period is dynamic or static
	bool _dynamic;

	//! Vector of possible period values (in increasing order) when the policy is
	//! dynamic.
	std::vector<uint64_t> _periods;

	//! The current position in the vector of period values
	size_t _position;

	//! Rolling window of last requests check results. This is used to take decisions
	//! regarding increasing, decreasing or maintaining the polling period
	RollingWindow _window;

public:
	//! \brief Create the frequency controller
	//!
	//! The controller reads the TAMPI_POLLING_PERIOD envar to determine the period in
	//! which TAMPI will check its MPI requests. If the envar is not defined, the period
	//! is 100us as a default value. Notice that TAMPI_POLLING_FREQUENCY is deprecated
	inline PollingPeriodCtrl() :
		_periodMin(100), _periodMax(100), _dynamic(false), _position(0), _window()
	{
		EnvironmentVariable<std::string> pollingPeriod("TAMPI_POLLING_PERIOD");
		EnvironmentVariable<uint64_t> pollingFrequency("TAMPI_POLLING_FREQUENCY");

		if (pollingFrequency.isPresent()) {
			ErrorHandler::warn("TAMPI_POLLING_FREQUENCY is deprecated; use TAMPI_POLLING_PERIOD");
		}

		// Give precedence to TAMPI_POLLING_PERIOD
		if (pollingPeriod.isPresent()) {
			std::stringstream stream(pollingPeriod.get());

			std::string component;
			std::vector<std::string> components;
			while (std::getline(stream, component, ':'))
				components.push_back(component);

			if (components.size() == 0 || components.size() > 2)
				ErrorHandler::fail("TAMPI_POLLING_PERIOD has format '<min period>[:<max period>]'");

			std::istringstream(components[0]) >> _periodMin;
			_periodMax = _periodMin;

			if (components.size() == 2)
				std::istringstream(components[1]) >> _periodMax;
		} else if (pollingFrequency.isPresent()) {
			_periodMin = pollingFrequency.get();
			_periodMax = pollingFrequency.get();
		}

		if (_periodMin > _periodMax)
			ErrorHandler::fail("Minimum polling period cannot be greater than maximum");

		_dynamic = (_periodMax != _periodMin);

		if (_dynamic) {
			uint64_t period = _periodMin;
			while (period < _periodMax) {
				_periods.push_back(period);
				period += INCREASE_STEP;
			}
			if (_periods[_periods.size() - 1] != _periodMax)
				_periods.push_back(_periodMax);
		}
	}

	//! \brief Retrieve the static period in microseconds
	//!
	//! \params completed The number of completed requests
	//! \params pending The number of pending requests
	inline uint64_t getPeriod(size_t completed, size_t pending)
	{
		if (!_dynamic)
			return _periodMax;

		if (pending == 0) {
			// No pending requests. Increase to the maximum period
			_window.reset();
			_position = _periods.size() - 1;
		} else {
			// Run the window and take a decision
			RollingWindow::Decision decision = _window.run(completed);
			if (decision == RollingWindow::INCREASE)
				_position = (_position < _periods.size() - 1) ? _position + 1 : _position;
			else if (decision == RollingWindow::DECREASE)
				_position = (_position > 0) ? _position - 1 : _position;
		}

		return _periods[_position];
	}
};

} // namespace tampi

#endif // POLLING_PERIOD_CTRL_HPP
