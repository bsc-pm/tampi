/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef OPERATION_HPP
#define OPERATION_HPP

#include <mpi.h>

#include <cassert>

#include "Interface.hpp"
#include "TaskContext.hpp"


namespace tampi {

enum opcode_t : char {
	NONE = 0,
	// Point-to-point operations
	BSEND,
	RECV,
	RSEND,
	SEND,
	SSEND,
	// Collective operations
	ALLGATHER,
	ALLGATHERV,
	ALLREDUCE,
	ALLTOALL,
	ALLTOALLV,
	ALLTOALLW,
	BARRIER,
	BCAST,
	EXSCAN,
	GATHER,
	GATHERV,
	REDUCE,
	REDUCESCATTER,
	REDUCESCATTERBLOCK,
	SCAN,
	SCATTER,
	SCATTERV,
};

template <typename Lang>
struct Operation {
	typedef typename Types<Lang>::int_t int_t;
	typedef typename Types<Lang>::datatype_t datatype_t;
	typedef typename Types<Lang>::comm_t comm_t;
	typedef typename Types<Lang>::request_t request_t;

	void *_buffer;
	int_t _count;
	datatype_t _datatype;
	comm_t _comm;
	int_t _rank;
	int_t _tag;
	opcode_t _opcode;

	Operation(opcode_t opcode, const void *buffer, int_t count,
		datatype_t datatype, int_t rank, int_t tag, comm_t comm
	) :
		_buffer(const_cast<void *>(buffer)), _count(count), _datatype(datatype),
		_comm(comm), _rank(rank), _tag(tag), _opcode(opcode)
	{
	}

	Operation() : _opcode(NONE)
	{
	}

	comm_t getComm() const
	{
		return _comm;
	}

	int_t getTag() const
	{
		return _tag;
	}

	request_t issue();
};

template <typename Lang>
struct CollOperation {
	typedef typename Types<Lang>::int_t int_t;
	typedef typename Types<Lang>::datatype_t datatype_t;
	typedef typename Types<Lang>::op_t op_t;
	typedef typename Types<Lang>::comm_t comm_t;
	typedef typename Types<Lang>::request_t request_t;

	const void *_sendbuf;
	void *_recvbuf;
	const int_t *_senddispls;
	const int_t *_recvdispls;
	union {
		int_t _sendcount;
		const int_t *_sendcounts;
	};
	union {
		int_t _recvcount;
		const int_t *_recvcounts;
	};
	union {
		datatype_t _sendtype;
		const datatype_t *_sendtypes;
	};
	union {
		datatype_t _recvtype;
		const datatype_t *_recvtypes;
	};
	comm_t _comm;
	int_t _rank;
	op_t _op;
	opcode_t _opcode;

	CollOperation(opcode_t opcode,
		const void *sendbuf, int_t sendcount, datatype_t sendtype,
		void *recvbuf, int_t recvcount, datatype_t recvtype,
		op_t op, int_t rank, comm_t comm
	) :
		_sendbuf(sendbuf), _recvbuf(recvbuf),
		_sendcount(sendcount), _recvcount(recvcount), _sendtype(sendtype),
		_recvtype(recvtype), _comm(comm), _rank(rank), _op(op), _opcode(opcode)
	{
	}

	CollOperation() : _opcode(NONE)
	{
	}

	comm_t getComm() const
	{
		return _comm;
	}

	int_t getTag() const
	{
		return 0;
	}

	request_t issue();
};

} // namespace tampi

#endif // OPERATION_HPP

