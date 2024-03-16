/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023-2024 Barcelona Supercomputing Center (BSC)
*/

#include "Interface.hpp"
#include "Operation.hpp"

namespace tampi {

template <>
Operation<C>::request_t Operation<C>::issue()
{
	int err = MPI_SUCCESS;
	request_t request = Interface<C>::REQUEST_NULL;

	switch (_opcode) {
		case SEND:
			err = Interface<C>::mpi_isend(_buffer, _count, _datatype, _rank, _tag, _comm, &request);
			break;
		case BSEND:
			err = Interface<C>::mpi_ibsend(_buffer, _count, _datatype, _rank, _tag, _comm, &request);
			break;
		case RSEND:
			err = Interface<C>::mpi_irsend(_buffer, _count, _datatype, _rank, _tag, _comm, &request);
			break;
		case SSEND:
			err = Interface<C>::mpi_issend(_buffer, _count, _datatype, _rank, _tag, _comm, &request);
			break;
		case RECV:
			err = Interface<C>::mpi_irecv(_buffer, _count, _datatype, _rank, _tag, _comm, &request);
			break;
		default:
			ErrorHandler::fail("Invalid operation ", _opcode);
			break;
	}

	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Operation ", _opcode, " failed");

	return request;
}

template <>
Operation<C>::request_t CollOperation<C>::issue()
{
	int err = MPI_SUCCESS;
	request_t request = Interface<C>::REQUEST_NULL;

	switch (_opcode) {
		case ALLGATHER:
			err = Interface<C>::mpi_iallgather(_sendbuf, _sendcount, _sendtype, _recvbuf, _recvcount, _recvtype, _comm, &request);
			break;
		case ALLGATHERV:
			err = Interface<C>::mpi_iallgatherv(_sendbuf, _sendcount, _sendtype, _recvbuf, _recvcounts, _recvdispls, _recvtype, _comm, &request);
			break;
		case ALLREDUCE:
			err = Interface<C>::mpi_iallreduce(_sendbuf, _recvbuf, _sendcount, _sendtype, _op, _comm, &request);
			break;
		case ALLTOALL:
			err = Interface<C>::mpi_ialltoall(_sendbuf, _sendcount, _sendtype, _recvbuf, _recvcount, _recvtype, _comm, &request);
			break;
		case ALLTOALLV:
			err = Interface<C>::mpi_ialltoallv(_sendbuf, _sendcounts, _senddispls, _sendtype, _recvbuf, _recvcounts, _recvdispls, _recvtype, _comm, &request);
			break;
		case ALLTOALLW:
			err = Interface<C>::mpi_ialltoallw(_sendbuf, _sendcounts, _senddispls, _sendtypes, _recvbuf, _recvcounts, _recvdispls, _recvtypes, _comm, &request);
			break;
		case BARRIER:
			err = Interface<C>::mpi_ibarrier(_comm, &request);
			break;
		case BCAST:
			err = Interface<C>::mpi_ibcast(_recvbuf, _recvcount, _recvtype, _rank, _comm, &request);
			break;
		case EXSCAN:
			err = Interface<C>::mpi_iexscan(_sendbuf, _recvbuf, _sendcount, _sendtype, _op, _comm, &request);
			break;
		case GATHER:
			err = Interface<C>::mpi_igather(_sendbuf, _sendcount, _sendtype, _recvbuf, _recvcount, _recvtype, _rank, _comm, &request);
			break;
		case GATHERV:
			err = Interface<C>::mpi_igatherv(_sendbuf, _sendcount, _sendtype, _recvbuf, _recvcounts, _recvdispls, _recvtype, _rank, _comm, &request);
			break;
		case REDUCE:
			err = Interface<C>::mpi_ireduce(_sendbuf, _recvbuf, _sendcount, _sendtype, _op, _rank, _comm, &request);
			break;
		case REDUCESCATTER:
			err = Interface<C>::mpi_ireduce_scatter(_sendbuf, _recvbuf, _recvcounts, _sendtype, _op, _comm, &request);
			break;
		case REDUCESCATTERBLOCK:
			err = Interface<C>::mpi_ireduce_scatter_block(_sendbuf, _recvbuf, _recvcount, _sendtype, _op, _comm, &request);
			break;
		case SCAN:
			err = Interface<C>::mpi_iscan(_sendbuf, _recvbuf, _sendcount, _sendtype, _op, _comm, &request);
			break;
		case SCATTER:
			err = Interface<C>::mpi_iscatter(_sendbuf, _sendcount, _sendtype, _recvbuf, _recvcount, _recvtype, _rank, _comm, &request);
			break;
		case SCATTERV:
			err = Interface<C>::mpi_iscatterv(_sendbuf, _sendcounts, _senddispls, _sendtype, _recvbuf, _recvcount, _recvtype, _rank, _comm, &request);
			break;
		default:
			ErrorHandler::fail("Invalid large operation ", _opcode);
			break;
	}

	if (err != MPI_SUCCESS)
		ErrorHandler::fail("Operation ", _opcode, " failed");

	return request;
}

template <>
Types<Fortran>::request_t Operation<Fortran>::issue()
{
	ErrorHandler::fail("Fortran not supported");
	return Interface<Fortran>::REQUEST_NULL;
}

template <>
Types<Fortran>::request_t CollOperation<Fortran>::issue()
{
	ErrorHandler::fail("Fortran not supported");
	return Interface<Fortran>::REQUEST_NULL;
}

} // namespace tampi
