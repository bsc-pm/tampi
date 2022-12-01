!
!	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
!
!	Copyright (C) 2019-2022 Barcelona Supercomputing Center (BSC)
!

#include "TAMPIf.h"

! NOTE: Do not take this test as an example, since
! it is not correct. It is only useful to check that
! compiles properly. Please do not execute it.

module test
implicit none
include "mpif.h"

contains

subroutine test_kernel
implicit none 

integer :: rank, nranks, provided, err
integer :: num, datatype, op, comm
integer, allocatable :: sendbuf(:), recvbuf(:), requests(:), statuses(:,:), aux(:), datatypes(:)

call MPI_Init_thread(MPI_TASK_MULTIPLE, provided, err)
if (provided < 100) then
    write(*,'("This code should not be executed!")')
    return
endif

call MPI_Comm_rank(MPI_COMM_WORLD, rank, err)
call MPI_Comm_size(MPI_COMM_WORLD, nranks, err)

call TAMPI_Ibsend(sendbuf, num, datatype, num, num, comm, requests(1), err)
call TAMPI_Irecv(recvbuf, num, datatype, num, num, comm, requests(1), statuses(:,1), err)
call TAMPI_Irsend(sendbuf, num, datatype, num, num, comm, requests(1), err)
call TAMPI_Isend(sendbuf, num, datatype, num, num, comm, requests(1), err)
call TAMPI_Issend(sendbuf, num, datatype, num, num, comm, requests(1), err)
call TAMPI_Iwait(requests(1), statuses(:,1), err)
call TAMPI_Iwaitall(num, requests(:), statuses(:,:), err)

call TAMPI_Iallgather(sendbuf, num, datatype, recvbuf, num, datatype, comm, requests(1), err)
call TAMPI_Iallgatherv(sendbuf, num, datatype, recvbuf, aux, aux, datatype, comm, requests(1), err)
call TAMPI_Iallreduce(sendbuf, recvbuf, num, datatype, op, comm, requests(1), err)
call TAMPI_ialltoall(sendbuf, num, datatype, recvbuf, num, datatype, comm, requests(1), err)
call TAMPI_ialltoallv(sendbuf, aux, aux, datatype, recvbuf, aux, aux, datatype, comm, requests(1), err)
call TAMPI_ialltoallw(sendbuf, aux, aux, datatypes(:), recvbuf, aux, aux, datatypes(:), comm, requests(1), err)
call TAMPI_Ibarrier(comm, requests(1), err)
call TAMPI_Ibcast(recvbuf, num, datatype, num, comm, requests(1), err)
call TAMPI_Igather(sendbuf, num, datatype, recvbuf, num, datatype, num, comm, requests(1), err)
call TAMPI_IGATHERV(sendbuf, num, datatype, recvbuf, aux, aux, datatype, num, comm, requests(1), err)
call TAMPI_Ireduce(sendbuf, recvbuf, num, datatype, op, num, comm, requests(1), err)
call TAMPI_Ireduce_scatter(sendbuf, recvbuf, aux, datatype, op, comm, requests(1), err)
call TAMPI_IREDUCE_SCATTER_BLOCK(sendbuf, recvbuf, num, datatype, op, comm, requests(1), err)
call TAMPI_Iscatter(sendbuf, num, datatype, recvbuf, num, datatype, num, comm, requests(1), err)
call TAMPI_Iscatterv(sendbuf, aux, aux, datatype, recvbuf, num, datatype, num, comm, requests(1), err)
call TAMPI_Iscan(sendbuf, recvbuf, num, datatype, op, comm, requests(1), err)
call TAMPI_Iexscan(sendbuf, recvbuf, num, datatype, op, comm, requests(1), err)
call TAMPI_Wait(requests(1), statuses(:,1), err)
call TAMPI_Waitall(num, requests(:), statuses(:,1), err)

call MPI_Finalize(err)

return
end subroutine test_kernel
end module test

program test_program
use test
call test_kernel
end program test_program
