#include <mpi.h>
#include <TAMPI.h>

#include "Utils.hpp"

#include <vector>

const int TIMESTEPS = 100;
const int MSG_NUM = 1000;
const int MSG_SIZE = 100;

int main(int argc, char **argv)
{
	int provided;
	const int required = MPI_TASK_MULTIPLE;
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
					CHECK(MPI_Send(message, MSG_SIZE, MPI_INT, 0, m, MPI_COMM_WORLD));
				}
				message += MSG_SIZE;
			}
		} else if (rank == 0) {
			int *message1 = buffer1 + (MSG_NUM - 1) * MSG_SIZE;
			int *message2 = buffer2 + (MSG_NUM - 1) * MSG_SIZE;
			int *message3 = buffer3 + (MSG_NUM - 1) * MSG_SIZE;
			
			for (int m = MSG_NUM - 1; m >= 0; --m) {
				#pragma oss task out(message1[0;MSG_SIZE], message2[0;MSG_SIZE], message3[0;MSG_SIZE]) label(recv)
				{
					MPI_Request requests[4];
					MPI_Status statuses[4];
					CHECK(MPI_Irecv(message1, MSG_SIZE, MPI_INT, 1, m, MPI_COMM_WORLD, &requests[0]));
					CHECK(MPI_Irecv(message2, MSG_SIZE, MPI_INT, 2, m, MPI_COMM_WORLD, &requests[2]));
					CHECK(MPI_Irecv(message3, MSG_SIZE, MPI_INT, 3, m, MPI_COMM_WORLD, &requests[3]));
					requests[1] = MPI_REQUEST_NULL;
					CHECK(MPI_Waitall(4, requests, statuses));
					
					int count1, count2, count3;
					CHECK(MPI_Get_count(&statuses[0], MPI_INT, &count1));
					CHECK(MPI_Get_count(&statuses[2], MPI_INT, &count2));
					CHECK(MPI_Get_count(&statuses[3], MPI_INT, &count3));
					ASSERT(count1 == MSG_SIZE);
					ASSERT(count2 == MSG_SIZE);
					ASSERT(count3 == MSG_SIZE);
					
					ASSERT(statuses[0].MPI_TAG == m);
					ASSERT(statuses[0].MPI_SOURCE == 1);
					ASSERT(statuses[2].MPI_TAG == m);
					ASSERT(statuses[2].MPI_SOURCE == 2);
					ASSERT(statuses[3].MPI_TAG == m);
					ASSERT(statuses[3].MPI_SOURCE == 3);
				}
				
				#pragma oss task in(message1[0;MSG_SIZE], message2[0;MSG_SIZE], message3[0;MSG_SIZE]) label(check)
				{
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
