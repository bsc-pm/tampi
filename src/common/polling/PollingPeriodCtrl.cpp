/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023-2024 Barcelona Supercomputing Center (BSC)
*/

#include "PollingPeriodCtrl.hpp"
#include "SlowStartPollingPeriodCtrl.hpp"
#include "util/ErrorHandler.hpp"
#include "util/StringSupport.hpp"


namespace tampi {

PollingPeriodCtrl::PollingPeriodCtrl(const std::string &envar) :
	_envar(envar, std::to_string(DefaultPeriodUs)),
	_minPeriod(0),
	_maxPeriod(0),
	_dynamic(false),
	_policy(""),
	_dynamicCtrl(nullptr)
{
	bool present[3];
	if (!StringSupport::parse(_envar.get(), present, _minPeriod, _maxPeriod, _policy, ':'))
		ErrorHandler::fail("Invalid format TAMPI_POLLING_PERIOD=minperiod[:maxperiod[:policy]]");

	if (!present[0])
		_minPeriod = DefaultPeriodUs;
	if (!present[1])
		_maxPeriod = _minPeriod;
	if (!present[2])
		_policy = DefaultPolicy;

	if (_minPeriod > _maxPeriod)
		ErrorHandler::fail("TAMPI_POLLING_PERIOD has a minperiod greater than maxperiod");

	if (_maxPeriod != _minPeriod) {
		_dynamic = true;

		if (_policy == "slowstart" || _policy == "default")
			_dynamicCtrl = new SlowStartPollingPeriodCtrl(_minPeriod, _maxPeriod);
		else
			ErrorHandler::fail("TAMPI_POLLING_PERIOD has an invalid policy '", _policy, "'");
	}
}

PollingPeriodCtrl::~PollingPeriodCtrl()
{
	if (_dynamicCtrl)
		delete _dynamicCtrl;
}

} // namespace tampi
