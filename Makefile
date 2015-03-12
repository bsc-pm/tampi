OMPI_CC=/home/jbellon/ompss/mcxx/bin/mcxx
CFLAGS=-k --ompss -O0 -g3 --debug -std=c++11
CPPFLAGS=-I/home/jbellon/ompss/nanox-master/include/nanox-dev

all: mpitest

mpitest: mpitest.cc
	OMPI_CC="$(OMPI_CC)" OMPI_CFLAGS="$(CFLAGS)" mpicc $(CPPFLAGS) $(CFLAGS) $+ -o $@

mpitest-thru: mpitest.cc
	OMPI_CC="$(OMPI_CC)" OMPI_CFLAGS="$(CFLAGS)" mpicc $(CPPFLAGS) $(CFLAGS) --pass-through $+ -o mpitest

run: mpitest
	NX_ARGS="--summary --smp-cpus=1 --spins=0 --enable-block --schedule=polling" mpirun -x NX_ARGS -np 2 ./mpitest

debug: mpitest
	NX_ARGS="--smp-cpus=1 --schedule=polling --verbose" mpirun -np 2 konsole -e gdb ./mpitest

clean:
	rm -f mpitest mcxx_mpitest.cc
