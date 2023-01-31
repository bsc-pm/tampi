/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef TICKET_MANAGER_CAPACITY_CTRL_HPP
#define TICKET_MANAGER_CAPACITY_CTRL_HPP

#include <cstdint>
#include <vector>
#include <sstream>
#include <string>

#include "util/Clock.hpp"
#include "util/EnvironmentVariable.hpp"

namespace tampi {

//! Class that controls the capacity of the ticket manager capacity
class TicketManagerCapacityCtrl {
private:
	//! Hard limit of in-flight requests
	static constexpr uint64_t CapacityLimit = 32*1024;

	//! The minimum and maximum capacity
	uint64_t _capacityMin;
	uint64_t _capacityMax;

	//! Indicate whether the capacity is dynamic
	bool _dynamic;

	//! The current capacity
	uint64_t _capacity;

	//! The maximum time (in ms) which a saturation epoch may take
	uint64_t _timeout;

	//! Indicate whether there is a saturation epoch currently
	bool _saturation;

	//! The timestamp (in ms) when the saturation epoch began (if any)
	double _saturationBegin;

public:
	TicketManagerCapacityCtrl() :
		_capacityMin(128),
		_capacityMax(CapacityLimit),
		_dynamic(true),
		_capacity(_capacityMin),
		_timeout(10),
		_saturation(false)
	{
		EnvironmentVariable<std::string> policy("TAMPI_CAPACITY");
		EnvironmentVariable<uint64_t> timeout("TAMPI_CAPACITY_TIMEOUT");

		if (policy.isPresent()) {
			auto [min, max] = parseCapacities(policy.get());
			if (min > max)
				ErrorHandler::fail("Minimum capacity cannot be greater than maximum");

			_capacityMin = std::min(min, CapacityLimit);
			_capacityMax = std::min(max, CapacityLimit);
			_capacity = _capacityMin;
			_dynamic = (_capacityMin != _capacityMax);
		}

		if (timeout.isPresent())
			_timeout = timeout.get();
	}

	inline int get() const
	{
		return _capacity;
	}

	constexpr static inline int max()
	{
		return CapacityLimit;
	}

	//! \brief Evaluate whether the capacity must be modified
	//!
	//! This function decides which should be capacity of the ticket manager,
	//! which is the maximum number of in-flight requests. To this end, the
	//! function considers the current number of pending requests and the
	//! number of completed requests since the last evaluation. This function
	//! should be called just before returning from the polling body.
	//!
	//! \param pending The number of pending requests
	//! \param completed The number of completed requests
	inline void evaluate(uint64_t pending, uint64_t completed)
	{
		// Nothing to do if the maximum capacity was reached
		if (_capacity == _capacityMax)
			return;

		// The saturation protocol should be enabled while we are in the maximum
		// capacity and we find no completed requests. Once this condition persists
		// for more than a timeout period (in ms), we need to increase the capacity.
		// Otherwise, we could end up in a communication deadlock
		if (pending == _capacity && completed == 0) {
			if (!_saturation) {
				// Enable saturation protocol
				_saturation = true;
				_saturationBegin = Clock::now_ms();
			} else if (Clock::now_ms() - _saturationBegin > _timeout) {
				// The saturation grace period expired. Double the capacity and
				// disable the saturation protocol
				_saturation = false;
				_capacity = std::min(_capacity * 2, _capacityMax);
				ErrorHandler::warn("Increasing capacity to ", _capacity);
			}
		} else {
			// Disable saturation protocol
			_saturation = false;
		}
	}

	//! \brief Parse the capacity policy string
	//!
	//! \param policy The policy string
	//!
	//! \returns a pair with the minimum and maximum capacities
	static inline std::pair<uint64_t, uint64_t> parseCapacities(
		const std::string &policy
	) {
		std::string component;
		std::vector<std::string> components;
		std::stringstream stream(policy);
		while (std::getline(stream, component, ':'))
			components.push_back(component);

		if (components.size() < 1 || components.size() > 2)
			ErrorHandler::fail("TAMPI_CAPACITY has format '<min capacity>[:<max capacity>]'");

		uint64_t min, max;
		std::istringstream(components[0]) >> min;
		if (components.size() > 1)
			std::istringstream(components[1]) >> max;
		else
			max = min;

		return { min, max };
	}
};

} // namespace tampi

#endif // TICKET_MANAGER_CAPACITY_CTRL_HPP
