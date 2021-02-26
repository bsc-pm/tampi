/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2021 Barcelona Supercomputing Center (BSC)
*/

#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <stdint.h>
#include <time.h>

#include "util/ErrorHandler.hpp"


namespace tampi {

//! Class that provides the features of a monotonic clock
class Clock {
public:
	//! \brief Get the current monotonic time (ns)
	static inline double now()
	{
		struct timespec tv;
		if (clock_gettime(CLOCK_MONOTONIC, &tv) != 0) {
			ErrorHandler::fail("Calling clock_gettime failed");
		}

		return (double)(tv.tv_sec) * 1.0e9 + (double)(tv.tv_nsec);
	}
};

} // namespace tampi

#endif // CLOCK_HPP
