# Compilers
CXX=clang++
F90=mfc
MPICXX?=mpicxx
MPIF90?=mpif90

# MPI Wrappers
CXX_WRAPPERS=I_MPI_CXX=$(CXX) MPICH_CXX=$(CXX) OMPI_CXX=$(CXX)
F90_WRAPPERS=I_MPI_F90=$(F90) MPICH_FC=$(F90) OMPI_FC=$(F90)

CPPFLAGS=-I.
CFLAGS=-O3 -std=c++11 --gcc-toolchain=$(shell which gcc | sed 's,/bin/gcc$$,,')
FFLAGS=-O3 --pp

ifdef LARGE_INPUT
CPPFLAGS+=-DLARGE_INPUT
endif

# TAMPI Flags
ifndef TAMPI_INCLUDE_PATH
ifndef TAMPI_HOME
$(error TAMPI_HOME must be defined if TAMPI_INCLUDE_PATH is not)
endif
TAMPI_INCLUDE_PATH=$(TAMPI_HOME)/include
endif

ifndef TAMPI_LIBRARY_PATH
ifndef TAMPI_HOME
$(error TAMPI_HOME must be defined if TAMPI_LIBRARY_PATH is not)
endif
TAMPI_LIBRARY_PATH=$(TAMPI_HOME)/lib
endif

TAMPI_CPPFLAGS=-I$(TAMPI_INCLUDE_PATH)
TAMPI_LDCFLAGS=-L$(TAMPI_LIBRARY_PATH) -ltampi-c -Wl,-rpath=$(TAMPI_LIBRARY_PATH)
TAMPI_LDFFLAGS=-L$(TAMPI_LIBRARY_PATH) -ltampi-fortran -Wl,-rpath=$(TAMPI_LIBRARY_PATH)

all: $(PROGS)

%.oss.nodes.test: oss/%.cpp
	@echo "  MPICXX $@"
	$(CXX_WRAPPERS) $(MPICXX) $(CPPFLAGS) $(TAMPI_CPPFLAGS) $(CFLAGS) -fompss-2=libnodes $^ -o $@ $(TAMPI_LDCFLAGS)

%.oss.nanos6.test: oss/%.cpp
	@echo "  MPICXX $@"
	$(CXX_WRAPPERS) $(MPICXX) $(CPPFLAGS) $(TAMPI_CPPFLAGS) $(CFLAGS) -fompss-2=libnanos6 $^ -o $@ $(TAMPI_LDCFLAGS)

%.oss.nodes.test: oss/%.F90
	@echo "  MPIF90 $@"
	$(F90_WRAPPERS) $(MPIF90) $(CPPFLAGS) $(FFLAGS) $(TAMPI_CPPFLAGS) --ompss-2=libnodes $^ -o $@ $(TAMPI_LDFFLAGS)

%.oss.nanos6.test: oss/%.F90
	@echo "  MPIF90 $@"
	$(F90_WRAPPERS) $(MPIF90) $(CPPFLAGS) $(FFLAGS) $(TAMPI_CPPFLAGS) --ompss-2=libnanos6 $^ -o $@ $(TAMPI_LDFFLAGS)

%.omp.test: omp/%.cpp
	@echo "  MPICXX $@"
	$(CXX_WRAPPERS) $(MPICXX) $(CPPFLAGS) $(CFLAGS) $(TAMPI_CPPFLAGS) -fopenmp=libompv $^ -o $@ $(TAMPI_LDCFLAGS)

clean:
	rm -f *.o *.test *.mod
