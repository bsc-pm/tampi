
#include <mpi.h>

#include "request.h"
#include "status.h"
#include "requestset.h"
#include "statusset.h"

using namespace nanos::mpi;

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);

	char bufs[50], bufr[50];

	nanos::mpi::request<MPI_Request> send_request, recv_request;

	MPI_Isend( bufs, 50, MPI_CHAR, 0, 0, MPI_COMM_SELF, send_request.data() );
	MPI_Irecv( bufr, 50, MPI_CHAR, 0, 0, MPI_COMM_SELF, recv_request.data() );

	bool wait;
	do {
		wait =
			send_request.test( MPI_STATUS_IGNORE ) &&
			recv_request.test( MPI_STATUS_IGNORE );
	} while( wait );

	MPI_Finalize();
}
