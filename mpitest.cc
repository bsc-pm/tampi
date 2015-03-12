#include <mpi.h>
#include <unistd.h>
#include <iostream>
#include <cassert>

#include <nanos.h>
#include "mpiconditionchecker.hpp"

#pragma omp task label(master)
void do_master( int sendMsg ) {
    int flag = 0;
    MPI_Status status;
    MPI_Request request;

    usleep(10000000);
    MPI_Isend( &sendMsg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &request );
    std::cout <<"Sent " << sendMsg << std::endl;

    nanos::MpiTestCond waitCond(&request, &status);
    nanos_sync_cond_wait( &waitCond );
}

#pragma omp task label(slave)
void do_slave( int *recvMsg ) {
    int flag = 0;
    MPI_Status status;
    MPI_Request request;

    //usleep(5000000);
    MPI_Irecv( recvMsg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request );
    std::cout << "Waiting for receive" << std::endl;

    nanos::MpiTestCond waitCond(&request, &status);
    nanos_polling_cond_wait( &waitCond );
    std::cout << "Slave received? value=" << *recvMsg << std::endl;
}

#pragma omp task label(dummy)
void dummy( int rank ) {
	std::cout <<"Proc " << rank <<  ": I'm doing something!" << std::endl;
   usleep(1000000);
}
 
int main( int argc, char * argv[] )
{
    int rank;
    int sendMsg = 123;
    int recvMsg = 0;
    //int provided;
    //MPI_Init_thread( &argc, &argv, MPI_THREAD_MULTIPLE, &provided );
    //assert( provided == MPI_THREAD_MULTIPLE );
    MPI_Init( &argc, &argv );
 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if(rank == 0)
        do_master( sendMsg );
    else {
        do_slave( &recvMsg );
        for( int i = 0; i < 15; i++ )
            dummy( rank );
    }
    #pragma omp taskwait
 
    MPI_Finalize();
}

