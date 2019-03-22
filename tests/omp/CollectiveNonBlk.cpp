#include <mpi.h>
#include <TAMPI.h>

#include "Utils.hpp"

#include <vector>

#ifdef LARGE_INPUT
const int TIMESTEPS = 500;
const int MSG_NUM = 1000;
const int MSG_SIZE = 100;
#else
const int TIMESTEPS = 100;
const int MSG_NUM = 100;
const int MSG_SIZE = 100;
#endif

MPI_Comm comms[MSG_NUM];

int main(int argc, char **argv)
{
	int provided;
	const int required = MPI_THREAD_MULTIPLE;
	CHECK(MPI_Init_thread(&argc, &argv, required, &provided));
	ASSERT(provided == required);
	
	int rank, size;
	CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
	CHECK(MPI_Comm_size(MPI_COMM_WORLD, &size));
	ASSERT(size > 1);
	
	for (int c = 0; c < MSG_NUM; ++c) {
		CHECK(MPI_Comm_dup(MPI_COMM_WORLD, &comms[c]));
	}
	
	int * const buffer = (int *) std::malloc(MSG_NUM * MSG_SIZE * sizeof(int));
	ASSERT(buffer != nullptr);
	
	#pragma omp parallel
	#pragma omp single
	{
		for (int t = 0; t < TIMESTEPS; ++t) {
			if (rank == 0) {
				int *message = buffer;
				
				for (int m = 0; m < MSG_NUM; ++m) {
					#pragma omp task depend(out: message[0:MSG_SIZE-1])
					for (int d = 0; d < MSG_SIZE; ++d) {
						message[d] = d;
					}
					
					#pragma omp task depend(in: message[0:MSG_SIZE-1])
					{
						MPI_Request request;
						CHECK(MPI_Ibcast(message, MSG_SIZE, MPI_INT, 0, comms[m], &request));
						CHECK(TAMPI_Iwait(&request, MPI_STATUS_IGNORE));
					}
					message += MSG_SIZE;
				}
			} else if (rank == 1) {
				int *message = buffer + (MSG_NUM - 1) * MSG_SIZE;
				
				for (int m = MSG_NUM - 1; m >= 0; --m) {
					#pragma omp task depend(out: message[0:MSG_SIZE-1])
					{
						MPI_Request request;
						CHECK(MPI_Ibcast(message, MSG_SIZE, MPI_INT, 0, comms[m], &request));
						CHECK(TAMPI_Iwait(&request, MPI_STATUS_IGNORE));
					}
					
					#pragma omp task depend(in: message[0:MSG_SIZE-1])
					for (int d = 0; d < MSG_SIZE; ++d) {
						ASSERT(message[d] == d);
					}
					message -= MSG_SIZE;
				}
			}
		}
		#pragma omp taskwait
	}
	
	CHECK(MPI_Barrier(MPI_COMM_WORLD));
	
	for (int c = 0; c < MSG_NUM; ++c) {
		CHECK(MPI_Comm_free(&comms[c]));
	}
	CHECK(MPI_Finalize());
	
	std::free(buffer);
	
	return 0;
}
