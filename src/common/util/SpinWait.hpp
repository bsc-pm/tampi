/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING file.

	Copyright (C) 2022-2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef SPIN_WAIT_HPP
#define SPIN_WAIT_HPP

#if defined(__powerpc__) || defined(__powerpc64__) || defined(__PPC__) || defined(__PPC64__) || defined(_ARCH_PPC) || defined(_ARCH_PPC64)
#define POWER9_ARCH
#endif

#if defined(__i386__) || defined(__x86_64__)
#define X86_ARCH
#endif

#if defined(__arm__) || defined(__aarch64__)
#define ARM_ARCH
#endif

#if defined(__SSE2__)
#include <xmmintrin.h>
#define SSE2_ARCH_FEATURE
#endif

#if defined(POWER9_ARCH)
/* Macros for adjusting thread priority (hardware multi-threading) */
#define HMT_very_low()    asm volatile("or 31,31,31   # very low priority")
#define HMT_low()         asm volatile("or 1,1,1	  # low priority")
#define HMT_medium_low()  asm volatile("or 6,6,6	  # medium low priority")
#define HMT_medium()      asm volatile("or 2,2,2	  # medium priority")
#define HMT_medium_high() asm volatile("or 5,5,5	  # medium high priority")
#define HMT_high()        asm volatile("or 3,3,3	  # high priority")
#define HMT_barrier()     asm volatile("" : : : "memory")
#endif

namespace tampi {

struct SpinWait {
	static void wait()
	{
#if defined(SSE2_ARCH_FEATURE)
		_mm_pause();
#elif defined(POWER9_ARCH)
		HMT_low();
#elif defined(X86_ARCH)
		asm volatile("pause" ::: "memory");
#elif defined(ARM_ARCH)
		__asm__ __volatile__ ("yield");
#else
		#pragma message ("No 'pause' instruction/intrisic found for this architecture ")
#endif
	}

	static void release()
	{
#if defined(POWER9_ARCH)
		HMT_medium();
#endif
	}
};

} // namespace tampi

#endif // SPIN_WAIT_HPP
