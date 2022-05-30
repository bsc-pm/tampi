/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2021-2022 Barcelona Supercomputing Center (BSC)
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
	static inline uint64_t now_ns()
	{
		struct timespec tv;
		if (clock_gettime(CLOCK_MONOTONIC, &tv) != 0) {
			ErrorHandler::fail("Calling clock_gettime failed");
		}

		return tv.tv_sec * 1000000000ULL + tv.tv_nsec;
	}

	//! \brief Get the current monotonic time (us)
	static inline uint64_t now_us()
	{
		struct timespec tv;
		if (clock_gettime(CLOCK_MONOTONIC, &tv) != 0) {
			ErrorHandler::fail("Calling clock_gettime failed");
		}

		return tv.tv_sec * 1000000ULL + tv.tv_nsec / 1000ULL;
	}
};

} // namespace tampi

#endif // CLOCK_HPP
