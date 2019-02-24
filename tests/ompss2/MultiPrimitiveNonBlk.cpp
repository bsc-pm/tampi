#include <mpi.h>
#include <TAMPI.h>

#include "Utils.hpp"

#include <vector>

const int TIMESTEPS = 1000;
const int MSG_NUM = 1000;
const int MSG_SIZE = 100;

struct statuses_t {
	MPI_Status status[4];
};

statuses_t statuses[MSG_NUM];

int main(int argc, char **argv)
{
	int provided;
	const int required = MPI_THREAD_MULTIPLE;
	CHECK(MPI_Init_thread(&argc, &argv, required, &provided));
	ASSERT(provided == required);
	
	int rank, size;
	CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
	CHECK(MPI_Comm_size(MPI_COMM_WORLD, &size));
	ASSERT(size >= 4);
	
	int * const buffer1 = (int *) std::malloc(MSG_NUM * MSG_SIZE * sizeof(int));
	ASSERT(buffer1 != nullptr);

	int * buffer2 = nullptr;
	int * buffer3 = nullptr;
	if (rank == 0) {
		buffer2 = (int *) std::malloc(MSG_NUM * MSG_SIZE * sizeof(int));
		buffer3 = (int *) std::malloc(MSG_NUM * MSG_SIZE * sizeof(int));
		ASSERT(buffer2 != nullptr);
		ASSERT(buffer3 != nullptr);
	}
	
	CHECK(MPI_Barrier(MPI_COMM_WORLD));
	double startTime = getTime();
	
	for (int t = 0; t < TIMESTEPS; ++t) {
		if (rank > 0) {
			int *message = buffer1;
			
			for (int m = 0; m < MSG_NUM; ++m) {
				#pragma oss task out(message[0;MSG_SIZE]) label(init)
				for (int d = 0; d < MSG_SIZE; ++d) {
					message[d] = d;
				}
				
				#pragma oss task in(message[0;MSG_SIZE]) label(send)
				{
					MPI_Request requests[2];
					requests[0] = MPI_REQUEST_NULL;
					CHECK(MPI_Isend(message, MSG_SIZE, MPI_INT, 0, m, MPI_COMM_WORLD, &requests[1]));
					CHECK(TAMPI_Iwaitall(2, requests, MPI_STATUSES_IGNORE));
				}
				message += MSG_SIZE;
			}
		} else if (rank == 0) {
			int *message1 = buffer1 + (MSG_NUM - 1) * MSG_SIZE;
			int *message2 = buffer2 + (MSG_NUM - 1) * MSG_SIZE;
			int *message3 = buffer3 + (MSG_NUM - 1) * MSG_SIZE;
			
			for (int m = MSG_NUM - 1; m >= 0; --m) {
				#pragma oss task label(recv) out(message1[0;MSG_SIZE], message2[0;MSG_SIZE], message3[0;MSG_SIZE]) out(statuses[m])
				{
					MPI_Request requests[4];
					CHECK(MPI_Irecv(message1, MSG_SIZE, MPI_INT, 1, m, MPI_COMM_WORLD, &requests[0]));
					CHECK(MPI_Irecv(message2, MSG_SIZE, MPI_INT, 2, m, MPI_COMM_WORLD, &requests[1]));
					CHECK(MPI_Irecv(message3, MSG_SIZE, MPI_INT, 3, m, MPI_COMM_WORLD, &requests[2]));
					requests[3] = MPI_REQUEST_NULL;
					CHECK(TAMPI_Iwaitall(4, requests, statuses[m].status));
				}
				
				#pragma oss task label(check) in(message1[0;MSG_SIZE], message2[0;MSG_SIZE], message3[0;MSG_SIZE]) in(statuses[m])
				{
					const MPI_Status &status1 = statuses[m].status[0];
					const MPI_Status &status2 = statuses[m].status[1];
					const MPI_Status &status3 = statuses[m].status[2];
					
					int count1, count2, count3;
					CHECK(MPI_Get_count(&status1, MPI_INT, &count1));
					CHECK(MPI_Get_count(&status2, MPI_INT, &count2));
					CHECK(MPI_Get_count(&status3, MPI_INT, &count3));
					ASSERT(count1 == MSG_SIZE);
					ASSERT(count2 == MSG_SIZE);
					ASSERT(count3 == MSG_SIZE);
					
					ASSERT(status1.MPI_TAG == m);
					ASSERT(status1.MPI_SOURCE == 1);
					ASSERT(status2.MPI_TAG == m);
					ASSERT(status2.MPI_SOURCE == 2);
					ASSERT(status3.MPI_TAG == m);
					ASSERT(status3.MPI_SOURCE == 3);
					
					for (int d = 0; d < MSG_SIZE; ++d) {
						ASSERT(message1[d] == d);
					}
					for (int d = 0; d < MSG_SIZE; ++d) {
						ASSERT(message2[d] == d);
					}
					for (int d = 0; d < MSG_SIZE; ++d) {
						ASSERT(message3[d] == d);
					}
				}
				
				message1 -= MSG_SIZE;
				message2 -= MSG_SIZE;
				message3 -= MSG_SIZE;
			}
		}
	}
	#pragma oss taskwait
	
	CHECK(MPI_Barrier(MPI_COMM_WORLD));
	
	if (rank == 0) {
		double endTime = getTime();
		fprintf(stdout, "Success, time: %f\n", endTime - startTime);
	}
	
	CHECK(MPI_Finalize());
	
	std::free(buffer1);
	if (rank == 0) {
		std::free(buffer2);
		std::free(buffer3);
	}
	
	return 0;
}
