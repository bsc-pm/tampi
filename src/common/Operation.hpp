/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef OPERATION_HPP
#define OPERATION_HPP

#include <mpi.h>

#include <cassert>

#include "Interface.hpp"
#include "TaskContext.hpp"


namespace tampi {

enum OpCode : char {
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

enum OpNature : char {
	BLK = 0,
	NONBLK
};

template <typename Lang>
struct Operation {
	typedef typename Types<Lang>::int_t int_t;
	typedef typename Types<Lang>::datatype_t datatype_t;
	typedef typename Types<Lang>::comm_t comm_t;
	typedef typename Types<Lang>::request_t request_t;
	typedef typename Types<Lang>::status_ptr_t status_ptr_t;

	TaskingModel::task_handle_t _task;
	status_ptr_t _status;
	void *_buffer;
	int_t _count;
	datatype_t _datatype;
	comm_t _comm;
	int_t _rank;
	int_t _tag;
	OpCode _code;
	OpNature _nature;

	Operation(TaskingModel::task_handle_t task, OpCode code, OpNature nature,
		const void *buffer, int_t count, datatype_t datatype, int_t rank,
		int_t tag, comm_t comm, status_ptr_t status = Interface<Lang>::STATUS_IGNORE
	) :
		_task(task), _status(status),
		_buffer(const_cast<void *>(buffer)), _count(count), _datatype(datatype),
		_comm(comm), _rank(rank), _tag(tag), _code(code), _nature(nature)
	{
	}

	Operation() : _code(NONE)
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

	TaskingModel::task_handle_t _task;
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
	OpCode _code;
	OpNature _nature;

	CollOperation(TaskingModel::task_handle_t task, OpCode code, OpNature nature,
		comm_t comm, const void *sendbuf, int_t sendcount, datatype_t sendtype,
		void *recvbuf, int_t recvcount, datatype_t recvtype,
		op_t op = MPI_OP_NULL, int_t rank = 0
	) :
		_task(task), _sendbuf(sendbuf), _recvbuf(recvbuf), _sendcount(sendcount),
		_recvcount(recvcount), _sendtype(sendtype), _recvtype(recvtype),
		_comm(comm), _rank(rank), _op(op), _code(code), _nature(nature)
	{
	}

	CollOperation(TaskingModel::task_handle_t task, OpCode code, OpNature nature,
		comm_t comm, const void *sendbuf, const int_t sendcounts[],
		const int_t senddispls[], datatype_t sendtype, void *recvbuf, int_t recvcount,
		datatype_t recvtype,
		op_t op = MPI_OP_NULL, int_t rank = 0
	) :
		_task(task), _sendbuf(sendbuf), _recvbuf(recvbuf), _senddispls(senddispls),
		_sendcounts(sendcounts), _recvcount(recvcount), _sendtype(sendtype),
		_recvtype(recvtype), _comm(comm), _rank(rank), _op(op),
		_code(code), _nature(nature)
	{
	}

	CollOperation(TaskingModel::task_handle_t task, OpCode code, OpNature nature,
		comm_t comm, const void *sendbuf, int_t sendcount, datatype_t sendtype,
		void *recvbuf, const int_t recvcounts[], const int_t recvdispls[],
		datatype_t recvtype, op_t op = MPI_OP_NULL, int_t rank = 0
	) :
		_task(task), _sendbuf(sendbuf), _recvbuf(recvbuf), _recvdispls(recvdispls),
		_sendcount(sendcount), _recvcounts(recvcounts), _sendtype(sendtype),
		_recvtype(recvtype), _comm(comm), _rank(rank), _op(op),
		_code(code), _nature(nature)
	{
	}

	CollOperation(TaskingModel::task_handle_t task, OpCode code, OpNature nature,
		comm_t comm, const void *sendbuf, const int_t sendcounts[],
		const int_t senddispls[], datatype_t sendtype, void *recvbuf,
		const int_t recvcounts[], const int_t recvdispls[], datatype_t recvtype,
		op_t op = MPI_OP_NULL, int_t rank = 0
	) :
		_task(task), _sendbuf(sendbuf), _recvbuf(recvbuf), _senddispls(senddispls),
		_recvdispls(recvdispls), _sendcounts(sendcounts), _recvcounts(recvcounts),
		_sendtype(sendtype), _recvtype(recvtype), _comm(comm), _rank(rank), _op(op),
		_code(code), _nature(nature)
	{
	}

	CollOperation(TaskingModel::task_handle_t task, OpCode code, OpNature nature,
		comm_t comm, const void *sendbuf, const int_t sendcounts[],
		const int_t senddispls[], const datatype_t sendtypes[],
		void *recvbuf, const int_t recvcounts[], const int_t recvdispls[],
		const datatype_t recvtypes[], op_t op = MPI_OP_NULL, int_t rank = 0
	) :
		_task(task), _sendbuf(sendbuf), _recvbuf(recvbuf), _senddispls(senddispls),
		_recvdispls(recvdispls), _sendcounts(sendcounts), _recvcounts(recvcounts),
		_sendtypes(sendtypes), _recvtypes(recvtypes), _comm(comm), _rank(rank), _op(op),
		_code(code), _nature(nature)
	{
	}

	CollOperation() : _code(NONE)
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

