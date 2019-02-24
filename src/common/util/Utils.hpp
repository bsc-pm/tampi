/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef UTILS_HPP
#define UTILS_HPP

#include <config.h>

#include "util/ArrayView.hpp"

#ifndef CACHELINE_SIZE
#define CACHELINE_SIZE 64
#endif

#ifndef SPIN_LOCK_READS
#define SPIN_LOCK_READS 1000
#endif

#ifndef MAX_SYSTEM_CPUS
#define MAX_SYSTEM_CPUS 50
#endif

namespace util {
	template<class T, size_t Size = CACHELINE_SIZE>
	class Padded : public T {
		constexpr static size_t roundup(size_t const x, size_t const y) {
			return ((((x) + ((y) - 1)) / (y)) * (y));
		}
		
		uint8_t padding[roundup(sizeof(T), Size)-sizeof(T)];
	};
	
	template<typename Lang>
	static inline int getActiveRequestCount(const util::ArrayView<typename Lang::request_t> &requests)
	{
		int active = 0;
		for (int r = 0; r < requests.size(); ++r) {
			if (requests[r] != Lang::REQUEST_NULL) {
				++active;
			}
		}
		return active;
	}
} // namespace util

#endif // UTILS_HPP
