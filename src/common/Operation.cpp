/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023-2024 Barcelona Supercomputing Center (BSC)
*/

#include "Operation.hpp"

namespace tampi {

template <>
Operation<C>::request_t Operation<C>::issue()
{
	int err = MPI_SUCCESS;
	request_t request = Interface<C>::REQUEST_NULL;

	switch (_opcode) {
		case SEND:
			err = PMPI_Isend(_buffer, _count, _datatype, _rank, _tag, _comm, &request);
			break;
		case BSEND:
			err = PMPI_Ibsend(_buffer, _count, _datatype, _rank, _tag, _comm, &request);
			break;
		case RSEND:
			err = PMPI_Irsend(_buffer, _count, _datatype, _rank, _tag, _comm, &request);
			break;
		case SSEND:
			err = PMPI_Issend(_buffer, _count, _datatype, _rank, _tag, _comm, &request);
			break;
		case RECV:
			err = PMPI_Irecv(_buffer, _count, _datatype, _rank, _tag, _comm, &request);
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
			err = MPI_Iallgather(_sendbuf, _sendcount, _sendtype, _recvbuf, _recvcount, _recvtype, _comm, &request);
			break;
		case ALLGATHERV:
			err = MPI_Iallgatherv(_sendbuf, _sendcount, _sendtype, _recvbuf, _recvcounts, _recvdispls, _recvtype, _comm, &request);
			break;
		case ALLREDUCE:
			err = MPI_Iallreduce(_sendbuf, _recvbuf, _sendcount, _sendtype, _op, _comm, &request);
			break;
		case ALLTOALL:
			err = MPI_Ialltoall(_sendbuf, _sendcount, _sendtype, _recvbuf, _recvcount, _recvtype, _comm, &request);
			break;
		case ALLTOALLV:
			err = MPI_Ialltoallv(_sendbuf, _sendcounts, _senddispls, _sendtype, _recvbuf, _recvcounts, _recvdispls, _recvtype, _comm, &request);
			break;
		case ALLTOALLW:
			err = MPI_Ialltoallw(_sendbuf, _sendcounts, _senddispls, _sendtypes, _recvbuf, _recvcounts, _recvdispls, _recvtypes, _comm, &request);
			break;
		case BARRIER:
			err = MPI_Ibarrier(_comm, &request);
			break;
		case BCAST:
			err = MPI_Ibcast(_recvbuf, _recvcount, _recvtype, _rank, _comm, &request);
			break;
		case EXSCAN:
			err = MPI_Iexscan(_sendbuf, _recvbuf, _sendcount, _sendtype, _op, _comm, &request);
			break;
		case GATHER:
			err = MPI_Igather(_sendbuf, _sendcount, _sendtype, _recvbuf, _recvcount, _recvtype, _rank, _comm, &request);
			break;
		case GATHERV:
			err = MPI_Igatherv(_sendbuf, _sendcount, _sendtype, _recvbuf, _recvcounts, _recvdispls, _recvtype, _rank, _comm, &request);
			break;
		case REDUCE:
			err = MPI_Ireduce(_sendbuf, _recvbuf, _sendcount, _sendtype, _op, _rank, _comm, &request);
			break;
		case REDUCESCATTER:
			err = MPI_Ireduce_scatter(_sendbuf, _recvbuf, _recvcounts, _sendtype, _op, _comm, &request);
			break;
		case REDUCESCATTERBLOCK:
			err = MPI_Ireduce_scatter_block(_sendbuf, _recvbuf, _recvcount, _sendtype, _op, _comm, &request);
			break;
		case SCAN:
			err = MPI_Iscan(_sendbuf, _recvbuf, _sendcount, _sendtype, _op, _comm, &request);
			break;
		case SCATTER:
			err = MPI_Iscatter(_sendbuf, _sendcount, _sendtype, _recvbuf, _recvcount, _recvtype, _rank, _comm, &request);
			break;
		case SCATTERV:
			err = MPI_Iscatterv(_sendbuf, _sendcounts, _senddispls, _sendtype, _recvbuf, _recvcount, _recvtype, _rank, _comm, &request);
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
