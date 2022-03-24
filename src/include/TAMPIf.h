!
!    This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
!
!    Copyright (C) 2015-2022 Barcelona Supercomputing Center (BSC)
!

#ifndef TAMPIF_H
#define TAMPIF_H

! MPI Threading level to enable the blocking mode of TAMPI
#define MPI_TASK_MULTIPLE 4


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Ibsend TAMPI_Ibsend_internal
#else
#define TAMPI_Ibsend MPI_Ibsend
#endif

#define TAMPI_IBSEND TAMPI_Ibsend
#define TAMPI_ibsend TAMPI_Ibsend
#define tampi_ibsend TAMPI_Ibsend


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Irecv TAMPI_Irecv_internal
#else
#define TAMPI_Irecv(buf, count, datatype, source, tag, comm, request, status, err) \
    MPI_Irecv(buf, count, datatype, source, tag, comm, request, err)
#endif

#define TAMPI_IRECV TAMPI_Irecv
#define TAMPI_irecv TAMPI_Irecv
#define tampi_irecv TAMPI_Irecv


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Irsend TAMPI_Irsend_internal
#else
#define TAMPI_Irsend MPI_Irsend
#endif

#define TAMPI_IRSEND TAMPI_Irsend
#define TAMPI_irsend TAMPI_Irsend
#define tampi_irsend TAMPI_Irsend


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Isend TAMPI_Isend_internal
#else
#define TAMPI_Isend MPI_Isend
#endif

#define TAMPI_ISEND TAMPI_Isend
#define TAMPI_isend TAMPI_Isend
#define tampi_isend TAMPI_Isend


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Issend TAMPI_Issend_internal
#else
#define TAMPI_Issend MPI_Issend
#endif

#define TAMPI_ISSEND TAMPI_Issend
#define TAMPI_issend TAMPI_Issend
#define tampi_issend TAMPI_Issend


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Iallgather TAMPI_Iallgather_internal
#else
#define TAMPI_Iallgather MPI_Iallgather
#endif

#define TAMPI_IALLGATHER TAMPI_Iallgather
#define TAMPI_iallgather TAMPI_Iallgather
#define tampi_iallgather TAMPI_Iallgather


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Iallgatherv TAMPI_Iallgatherv_internal
#else
#define TAMPI_Iallgatherv MPI_Iallgatherv
#endif

#define TAMPI_IALLGATHERV TAMPI_Iallgatherv
#define TAMPI_iallgatherv TAMPI_Iallgatherv
#define tampi_iallgatherv TAMPI_Iallgatherv


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Iallreduce TAMPI_Iallreduce_internal
#else
#define TAMPI_Iallreduce MPI_Iallreduce
#endif

#define TAMPI_IALLREDUCE TAMPI_Iallreduce
#define TAMPI_iallreduce TAMPI_Iallreduce
#define tampi_iallreduce TAMPI_Iallreduce


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Ialltoall TAMPI_Ialltoall_internal
#else
#define TAMPI_Ialltoall MPI_Ialltoall
#endif

#define TAMPI_IALLTOALL TAMPI_Ialltoall
#define TAMPI_ialltoall TAMPI_Ialltoall
#define tampi_ialltoall TAMPI_Ialltoall


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Ialltoallv TAMPI_Ialltoallv_internal
#else
#define TAMPI_Ialltoallv MPI_Ialltoallv
#endif

#define TAMPI_IALLTOALLV TAMPI_Ialltoallv
#define TAMPI_ialltoallv TAMPI_Ialltoallv
#define tampi_ialltoallv TAMPI_Ialltoallv


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Ialltoallw TAMPI_Ialltoallw_internal
#else
#define TAMPI_Ialltoallw MPI_Ialltoallw
#endif

#define TAMPI_IALLTOALLW TAMPI_Ialltoallw
#define TAMPI_ialltoallw TAMPI_Ialltoallw
#define tampi_ialltoallw TAMPI_Ialltoallw


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Ibarrier TAMPI_Ibarrier_internal
#else
#define TAMPI_Ibarrier MPI_Ibarrier
#endif

#define TAMPI_IBARRIER TAMPI_Ibarrier
#define TAMPI_ibarrier TAMPI_Ibarrier
#define tampi_ibarrier TAMPI_Ibarrier


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Ibcast TAMPI_Ibcast_internal
#else
#define TAMPI_Ibcast MPI_Ibcast
#endif

#define TAMPI_IBCAST TAMPI_Ibcast
#define TAMPI_ibcast TAMPI_Ibcast
#define tampi_ibcast TAMPI_Ibcast


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Igather TAMPI_Igather_internal
#else
#define TAMPI_Igather MPI_Igather
#endif

#define TAMPI_IGATHER TAMPI_Igather
#define TAMPI_igather TAMPI_Igather
#define tampi_igather TAMPI_Igather


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Igatherv TAMPI_Igatherv_internal
#else
#define TAMPI_Igatherv MPI_Igatherv
#endif

#define TAMPI_IGATHERV TAMPI_Igatherv
#define TAMPI_igatherv TAMPI_Igatherv
#define tampi_igatherv TAMPI_Igatherv


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Ireduce TAMPI_Ireduce_internal
#else
#define TAMPI_Ireduce MPI_Ireduce
#endif

#define TAMPI_IREDUCE TAMPI_Ireduce
#define TAMPI_ireduce TAMPI_Ireduce
#define tampi_ireduce TAMPI_Ireduce


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Ireduce_scatter TAMPI_Ireduce_scatter_internal
#else
#define TAMPI_Ireduce_scatter MPI_Ireduce_scatter
#endif

#define TAMPI_IREDUCE_SCATTER TAMPI_Ireduce_scatter
#define TAMPI_ireduce_scatter TAMPI_Ireduce_scatter
#define tampi_ireduce_scatter TAMPI_Ireduce_scatter


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Ireduce_scatter_block TAMPI_Ireduce_scatter_block_internal
#else
#define TAMPI_Ireduce_scatter_block MPI_Ireduce_scatter_block
#endif

#define TAMPI_IREDUCE_SCATTER_BLOCK TAMPI_Ireduce_scatter_block
#define TAMPI_ireduce_scatter_block TAMPI_Ireduce_scatter_block
#define tampi_ireduce_scatter_block TAMPI_Ireduce_scatter_block


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Iscatter TAMPI_Iscatter_internal
#else
#define TAMPI_Iscatter MPI_Iscatter
#endif

#define TAMPI_ISCATTER TAMPI_Iscatter
#define TAMPI_iscatter TAMPI_Iscatter
#define tampi_iscatter TAMPI_Iscatter


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Iscatterv TAMPI_Iscatterv_internal
#else
#define TAMPI_Iscatterv MPI_Iscatterv
#endif

#define TAMPI_ISCATTERV TAMPI_Iscatterv
#define TAMPI_iscatterv TAMPI_Iscatterv
#define tampi_iscatterv TAMPI_Iscatterv


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Iscan TAMPI_Iscan_internal
#else
#define TAMPI_Iscan MPI_Iscan
#endif

#define TAMPI_ISCAN TAMPI_Iscan
#define TAMPI_iscan TAMPI_Iscan
#define tampi_iscan TAMPI_Iscan


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Iexscan TAMPI_Iexscan_internal
#else
#define TAMPI_Iexscan MPI_Iexscan
#endif

#define TAMPI_IEXSCAN TAMPI_Iexscan
#define TAMPI_iexscan TAMPI_Iexscan
#define tampi_iexscan TAMPI_Iexscan


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Wait TAMPI_Wait_internal
#else
#define TAMPI_Wait MPI_Wait
#endif

#define TAMPI_WAIT TAMPI_Wait
#define TAMPI_wait TAMPI_Wait
#define tampi_wait TAMPI_Wait


#if defined(_OMPSS_2) || defined(_OPENMP)
#define TAMPI_Waitall TAMPI_Waitall_internal
#else
#define TAMPI_Waitall MPI_Waitall
#endif

#define TAMPI_WAITALL TAMPI_Waitall
#define TAMPI_waitall TAMPI_Waitall
#define tampi_waitall TAMPI_Waitall

#endif
