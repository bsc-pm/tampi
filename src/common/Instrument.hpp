/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2021 Barcelona Supercomputing Center (BSC)
*/

#ifndef INSTRUMENT_HPP
#define INSTRUMENT_HPP

#include <cmath>
#include <cstddef>
#include <cstdint>

#include "TaskingModel.hpp"
#include "util/Clock.hpp"

namespace tampi {

class Instrument {
private:
	//! Envar controlling whether the instrumentation should be enabled. The
	//! envar is called TAMPI_INSTRUMENT and is set to true by default
	static EnvironmentVariable<bool> _instrumentEnabled;

	//! Envar controlling the number of iterations during the synchronization
	//! mechanism when the instrumentation is enabled. The envar is called
	//! TAMPI_INSTRUMENT_SYNC_ITERATIONS and is set to 10 by default
	static EnvironmentVariable<size_t> _instrumentSyncIterations;

public:
	//! \brief Initialize the instrumentation
	static inline void initialize()
	{
		if (!_instrumentEnabled)
			return;

		if (TaskingModel::isDistributedInstrumentEnabled()) {
			int rank, nranks;
			MPI_Comm_size(MPI_COMM_WORLD, &nranks);
			MPI_Comm_rank(MPI_COMM_WORLD, &rank);

			int64_t startTime = TaskingModel::getInstrumentStartTime();
			assert(startTime > 0);

			double stdev;
			int64_t offset;
			const size_t nsamples = _instrumentSyncIterations;

			computeClockOffset(rank, startTime, nsamples, offset, stdev);

			TaskingModel::setupDistributedInstrument(rank, nranks, offset, nsamples, stdev);

			// Peform a barrier to ensure all ranks finished the setup
			PMPI_Barrier(MPI_COMM_WORLD);
		}
	}

private:
	//! \brief Compute the clock offset respecting the reference clock
	//!
	//! \param rank The current rank
	//! \param startTime The instrument start time of the current rank
	//! \param nsamples The number of samples to compute the offset
	//! \param offset The clock offset for the current rank (ns)
	//! \param stdev The resulting stdev of the offset (ns)
	static inline void computeClockOffset(
		size_t rank, int64_t startTime, size_t nsamples,
		int64_t &offset, double &stdev
	) {
		double *times = (double *) calloc(nsamples, sizeof(double));
		assert(times != nullptr);

		for (size_t s = 0; s < nsamples; s++) {
			PMPI_Barrier(MPI_COMM_WORLD);
			times[s] = Clock::now();
		}

		int64_t refStartTime = startTime;

		// The clock of rank 0 is the reference
		if (rank == 0) {
			PMPI_Bcast(&refStartTime, 1, MPI_INT64_T, 0, MPI_COMM_WORLD);
			PMPI_Bcast(times, nsamples, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			offset = 0;
			stdev = 0.0;
		} else {
			double *refTimes = (double *) calloc(nsamples, sizeof(double));
			assert(refTimes != nullptr);

			// Get the start time of the instrumentation of the reference
			// rank as seen by the reference clock
			PMPI_Bcast(&refStartTime, 1, MPI_INT64_T, 0, MPI_COMM_WORLD);

			// Get the times of the synchronization mechanism
			PMPI_Bcast(refTimes, nsamples, MPI_DOUBLE, 0, MPI_COMM_WORLD);

			double *deltas = (double *) calloc(nsamples, sizeof(double));
			assert(deltas);

			double sum = 0.0;
			for (size_t s = 0; s < nsamples; s++) {
				deltas[s] = refTimes[s] - times[s];
				sum += deltas[s];
			}
			const double mean = sum / nsamples;

			// Sort deltas
			std::sort(deltas, deltas + nsamples);
			const double median = deltas[nsamples / 2];

			double var = 0.0;
			for (size_t s = 0; s < nsamples; s++) {
				var += (deltas[s] - mean) * (deltas[s] - mean);
			}
			var /= nsamples - 1;
			stdev = std::sqrt(var);

			// Approximate the boot time b1 of the current rank as seen
			// from the reference monotonic clock m0(b1) by using the median between
			// the differences observed in both clocks after the barriers
			const double m0b1 = median;

			// The start time (s0 and s1) of the runtime instrumentation as seen
			// both from their own reference rank clock: m0(s0) and m1(s1)
			const double m0s0 = (double) refStartTime;
			const double m1s1 = (double) startTime;

			// The start time of the runtime instrumentation s1 of the current
			// rank as seen by the reference clock m0(s1): adding the boot time
			// offset of the current rank m0(b1) and the start time of the current
			// rank's instrumentation s1 from the current rank clock m1(s1)
			const double m0s1 = m0b1 + m1s1;

			// The offset is the difference between both starts as seen by the
			// reference clock
			offset = m0s1 - m0s0;

			free(refTimes);
			free(deltas);
		}

		// Move the offsets so that the minimum is zero
		int64_t minimum;
		PMPI_Allreduce(&offset, &minimum, 1, MPI_INT64_T, MPI_MIN, MPI_COMM_WORLD);
		offset -= minimum;
		assert(offset >= 0);
	}
};

} // namespace tampi

#endif // INSTRUMENT_HPP
