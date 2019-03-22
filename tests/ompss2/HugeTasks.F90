#include "TAMPIf.h"

module test
implicit none
include "mpif.h"

contains

subroutine test_kernel
implicit none 

integer :: err, provided, msg_size, msg_num, total_size
integer :: rank, nranks, timesteps, t, m, d, tmp
integer, allocatable :: buffer(:,:), requests(:), statuses(:,:)
real(kind=8) :: start_time, end_time

call MPI_Init_thread(MPI_TASK_MULTIPLE, provided, err)
if (provided /= MPI_TASK_MULTIPLE) then
    write(*,'("Error: TAMPI not supported")')
    return
endif

call MPI_Comm_rank(MPI_COMM_WORLD, rank, err)
call MPI_Comm_size(MPI_COMM_WORLD, nranks, err)

#ifdef LARGE_INPUT
timesteps = 1000;
msg_num = 1000;
msg_size = 100;
#else
timesteps = 500;
msg_num = 500;
msg_size = 100;
#endif

total_size = msg_num * msg_size;

allocate(buffer(msg_size, msg_num))
allocate(requests(msg_num))
allocate(statuses(MPI_STATUS_SIZE, msg_num))

call MPI_Barrier(MPI_COMM_WORLD, err)

start_time = MPI_Wtime()

do t = 1, timesteps
  if (rank == 0) then
    !$OSS TASK LABEL(init) DEFAULT(shared) PRIVATE(m,d) FIRSTPRIVATE(t) OUT(buffer(:,:))
    do m = 1, msg_num
      do d = 1, msg_size
        buffer(d, m) = d + t
      enddo
    enddo
    !$OSS END TASK
    
    !$OSS TASK LABEL(init) DEFAULT(shared) PRIVATE(m,d,err) IN(buffer(:,:))
    do m = 1, msg_num
      call MPI_Isend(buffer(:, m), msg_size, MPI_INTEGER, 1, m, MPI_COMM_WORLD, requests(m), err)
    enddo
    call MPI_Waitall(msg_num, requests, MPI_STATUSES_IGNORE, err)
    !$OSS END TASK
  
  else if (rank == 1) then
    !$OSS TASK LABEL(recv) DEFAULT(shared) PRIVATE(m,d,err) OUT(buffer(:,:), statuses(:,:))
    do m = 1, msg_num
      call MPI_Irecv(buffer(:, m), msg_size, MPI_INTEGER, 0, m, MPI_COMM_WORLD, requests(m), err)
    enddo
    call TAMPI_Iwaitall(msg_num, requests, statuses, err)
    !$OSS END TASK
    
    !$OSS TASK LABEL(init) DEFAULT(shared) PRIVATE(m,d,tmp,err) FIRSTPRIVATE(t) IN(buffer(:,:), statuses(:,:))
    do m = 1, msg_num
      if (statuses(MPI_SOURCE, m) /= 0) then
        write(*,'("Error: Wrong source")')
        stop
      endif
      if (statuses(MPI_TAG, m) /= m) then
        write(*,'("Error: Wrong tag")')
        stop
      endif
      call MPI_Get_count(statuses(:, m), MPI_INTEGER, tmp, err)
      if (tmp /= msg_size) then
        write(*,'("Error: Wrong message size")')
        stop
      endif
      
      do d = 1, msg_size
        if (buffer(d, m) /= d + t) then
          write(*,'("Error: Wrong result")')
          stop
        endif
      enddo
    enddo
    !$OSS END TASK
  endif
enddo

!$OSS TASKWAIT

call MPI_Barrier(MPI_COMM_WORLD, err)

end_time = MPI_Wtime()

if (rank == 0) then
    write(*, 1001) end_time - start_time
    1001 format('Success, time: ',F9.3)
end if

deallocate(buffer)
deallocate(requests)
deallocate(statuses)

call MPI_Finalize(err)

return
end subroutine test_kernel
end module test

program test_program
use test
call test_kernel
end program test_program
