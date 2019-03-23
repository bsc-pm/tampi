#!/bin/bash

function usage {
	if [ "$#" -eq 1 ]; then
		echo "Error: $1"
		echo ""
	fi
	echo "Usage: ./run-ompss2-tests.sh [OPTION]..."
	echo "  -l, --large              use large inputs (recommended only for powerful machines)"
	echo "  -p, --path     PATH      path to the TAMPI installation (default: read \$TAMPI_HOME env. variable)"
	echo "  -I, --includes INC_PATH  path to the directory containing the TAMPI headers (default: PATH/include)"
	echo "  -L, --libs     LIB_PATH  path to the directory containing the TAMPI libraries (default: PATH/lib)"
	echo "  -h, --help               display this help and exit"
	echo ""
	echo "Note: Make sure all MPI and OmpSs-2 binaries are available (i.e. through the \$PATH env. variable) when"
	echo "      executing this script"
	echo ""
	if [ "$#" -eq 1 ]; then
		exit 1
	fi
}

function find_binary {
	which $1 &> /dev/null
	if [ $? -eq 0 ]; then
		echo 1
	else
		echo 0
	fi
}

function check_binaries {
	nbins=$#
	bins=("$@")
	
	i=0
	while (("$i" < "$nbins")); do
		bin=${bins[$i]}
		((i++))
		
		which $bin &> /dev/null
		if [ $? -ne 0 ]; then
			usage "$bin binary not found"
		fi
	done
}

makefile=./Makefile.ompss2
if [ ! -f $makefile ]; then
	usage "Makefile $makefile not found! Execute this script from the './tampi/tests' folder."
fi

red="\033[0;31m"
green="\033[0;32m"
clean="\033[0m"

# Default parameters
mpi=mpich
large_input=0
tampi_path=
tampi_inc_path=
tampi_lib_path=

nargs=$#
args=("$@")

i=0
while (("$i" < "$nargs")); do
	opt=${args[$i]}
	((i++))
	
	# Options without argument
	if [ "$opt" == "-l" ] || [ "$opt" == "--large" ]; then
		large_input=1
		continue
	elif [ "$opt" == "-h" ] || [ "$opt" == "--help" ]; then
		usage
		exit 0
	fi
	
	if [ $i -eq $nargs ]; then
		usage "No argument for option $opt"
	fi
	
	val=${args[$i]}
	((i++))
	
	# Options with argument
	if [ "$opt" == "-p" ] || [ "$opt" == "--path" ]; then
		tampi_path=$val
	elif [ "$opt" == "-I" ] || [ "$opt" == "--includes" ]; then
		tampi_inc_path=$val
	elif [ "$opt" == "-L" ] || [ "$opt" == "--libs" ]; then
		tampi_lib_path=$val
	else
		usage "'$opt' is not a valid option!"
	fi
done


# TAMPI Installation
if [ -z "$tampi_path" ]; then
	if [ -z "$TAMPI_HOME" ]; then
		if [ -z "$tampi_inc_path" ] || [ -z "$tampi_lib_path" ]; then
			usage "Specify a TAMPI installation"
		fi
	fi
	tampi_path=$TAMPI_HOME
fi

if [ -z "$tampi_inc_path" ]; then
	tampi_inc_path=$tampi_path/include
fi
if [ -z "$tampi_lib_path" ]; then
	tampi_lib_path=$tampi_path/lib
fi

# MPI Binaries
mpicxx=mpiicpc
found=$(find_binary $mpicxx)
if [ $found -ne 1 ]; then
	mpicxx=mpicxx
fi

mpif90=mpiifort
found=$(find_binary $mpif90)
if [ $found -ne 1 ]; then
	mpif90=mpif90
fi

mpiexec=mpiexec.hydra
found=$(find_binary $mpiexec)
if [ $found -ne 1 ]; then
	mpiexec=mpiexec
fi
found=$(find_binary $mpiexec)
if [ $found -ne 1 ]; then
	mpiexec=mpirun
fi

check_binaries $mpicxx $mpif90 $mpiexec mcxx mfc

echo "---------------------------------------"
echo "TAMPI TEST SUITE"
echo "---------------------------------------"
echo ""
echo "Using TAMPI from:"
echo "  Headers:   $tampi_inc_path"
echo "  Libraries: $tampi_lib_path"
echo ""
echo "Using MPI from:"
echo "  C++:       $(which $mpicxx)"
echo "  Fortran:   $(which $mpif90)"
echo "  Launcher:  $(which $mpiexec)"
echo ""
echo "Using OmpSs-2 from:"
echo "  mcxx:      $(which mcxx)"
echo "  mfc:       $(which mfc)"
echo ""
echo "Using input data:"
if [ $large_input -eq 1 ]; then
	echo "  Large"
else
	echo "  Normal"
fi
echo ""

echo "Compiling tests..."
compile_args="TAMPI_INCLUDE_PATH=$tampi_inc_path TAMPI_LIBRARY_PATH=$tampi_lib_path MPICXX=$mpicxx MPIF90=$mpif90"
if [ $large_input -eq 1 ]; then
	compile_args="$compile_args LARGE_INPUT=1"
fi

make -f $makefile -B -s $compile_args
if [ $? -ne 0 ]; then
	exit 1
fi

progs=(
	PrimitiveBlk.ompss2.test
	PrimitiveNonBlk.ompss2.test
	PrimitiveWrappers.ompss2.test
	MultiPrimitiveBlk.ompss2.test
	MultiPrimitiveNonBlk.ompss2.test
	CollectiveBlk.ompss2.test
	CollectiveNonBlk.ompss2.test
	HugeBlkTasks.ompss2.test
	HugeTasks.ompss2.test
)

nprocsxprog=(
	2 # PrimitiveBlk.ompss2.test
	2 # PrimitiveNonBlk.ompss2.test
	2 # PrimitiveWrappers.ompss2.test
	4 # MultiPrimitiveBlk.ompss2.test
	4 # MultiPrimitiveNonBlk.ompss2.test
	4 # CollectiveBlk.ompss2.test
	4 # CollectiveNonBlk.ompss2.test
	2 # HugeBlkTasks.ompss2.test
	2 # HugeTasks.ompss2.test
)

nprogs=${#progs[@]}
nfailed=0

echo "---------------------------------------"
echo "TOTAL TESTS: $nprogs"
echo "---------------------------------------"

for ((i=0;i<nprogs;i++)); do
	prog=${progs[$i]}
	nprocs=${nprocsxprog[$i]}
	
	echo -n "${prog} "
	
	${mpiexec} -np ${nprocs} ./${prog} &> /dev/null
	if [ $? -eq 0 ]; then
		echo -e "${green}PASSED${clean}"
	else
		echo -e "${red}FAILED${clean}"
		nfailed=$(($nfailed + 1))
	fi
done

make -f $makefile clean -B -s $compile_args
if [ $? -ne 0 ]; then
	exit 1
fi

echo "---------------------------------------"
if [ $nfailed -eq 0 ]; then
	echo -e "SUMMARY: ${green}ALL TESTS PASSED${clean}"
	err=0
else
	echo -e "SUMMARY: ${red}${nfailed}/${nprogs} TESTS FAILED${clean}"
	err=1
fi
echo "---------------------------------------"

exit $err

