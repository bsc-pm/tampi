/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <time.h>

#ifdef CLOCK_MONOTONIC_COARSE
#define CLK_SRC_FAST CLOCK_MONOTONIC_COARSE
#else
#define CLK_SRC_FAST CLOCK_MONOTONIC
#endif

namespace tampi {

//! Class that provides clock features
class Clock {
public:
	static inline double now_ms()
	{
		struct timespec tp;
		clock_gettime(CLK_SRC_FAST, &tp);
		return (double)(tp.tv_sec) * 1.0e3 + (double)(tp.tv_nsec) * 1.0e-6;
	}
};

} // namespace tampi

#endif // CLOCK_HPP
