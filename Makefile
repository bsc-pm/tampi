MCC_HOME=$(HOME)/ompss/mcxx-nonblock-mpi
NANOS_HOME=$(HOME)/ompss/nanox-nonblock-mpi

pwd=/home/bsc15/bsc15685/tests/mpi-nonblocking

MCC=$(MCC_HOME)/bin/mcxx
CC=g++

OMPSS_FLAGS=-k --ompss --debug
NANOX_INC=-I$(NANOS_HOME)/include/nanox -I$(NANOS_HOME)/include/nanox-dev

CXXFLAGS=-O0 -g3 -std=c++11
CPPFLAGS=

MPI_CPPFLAGS=-I/apps/OPENMPI/1.8.1-mellanox/MULTITHREAD/include 
MPI_CXXFLAGS=-pthread 
MPI_LDFLAGS=-Wl,-rpath -Wl,/apps/OPENMPI/1.8.1-mellanox/MULTITHREAD/lib -Wl,--enable-new-dtags -L/apps/OPENMPI/1.8.1-mellanox/MULTITHREAD/lib -lmpi_cxx -lmpi

MPI_FLAGS=$(MPI_CPPFLAGS) $(MPI_CXXFLAGS) $(MPI_LDFLAGS)

all: libmpioverload.so mpitest

libmpioverload.so: mpioverload.c
	$(CC) $(MPI_FLAGS) $(CXXFLAGS) $(NANOX_INC) -fPIC --shared $+ -o $@

mpitest: mpitest.cc
	$(MCC) $(MPI_FLAGS) $(CPPFLAGS) $(CxXFLAGS) $(LDFLAGS) $+ -o $@

run-nonblock: mpitest
	LD_PRELOAD="$(pwd)/libmpioverload.so" NX_ARGS="--summary --smp-cpus=1 --schedule=polling" mpirun -np 2 ./mpitest

run-block: mpitest
	NX_ARGS="--summary --smp-cpus=1 --schedule=polling" mpirun -np 2 ./mpitest

debug: mpitest
	LD_PRELOAD="$(pwd)/libmpioverload.so" NX_ARGS="--smp-cpus=1 --schedule=polling --verbose" mpirun -np 2 konsole -e gdb ./mpitest

clean:
	rm -f mpitest mcxx_mpitest.cc libmpioverload.so
