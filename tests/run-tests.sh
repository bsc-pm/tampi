#!/bin/bash
#	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
#
#	Copyright (C) 2023 Barcelona Supercomputing Center (BSC)

function usage {
	if [ "$#" -eq 1 ]; then
		echo "Error: $1"
		echo ""
	fi
	echo "Usage: $0 [OPTION]..."
	echo "  -l, --large              use large inputs (recommended only for powerful machines)"
	echo "  -p, --path     PATH      path to the TAMPI installation (default: read \$TAMPI_HOME env. variable)"
	echo "  -I, --includes INC_PATH  path to the directory containing the TAMPI headers (default: PATH/include)"
	echo "  -L, --libs     LIB_PATH  path to the directory containing the TAMPI libraries (default: PATH/lib)"
	echo "  --slurm                  use SLURM srun for launching the tests. The scripts assumes it is running"
	echo "                           inside a sbatch or salloc session where the number of nodes, tasks and cpus"
	echo "                           per task has been defined"
	echo "  --skip-omp               skip execution of OpenMP tests"
	echo "  --keep                   do not remove binaries after executing"
	echo "  -h, --help               display this help and exit"
	echo ""
	echo "Note: Make sure all MPI and OmpSs-2 binaries are available (i.e. through the \$PATH env. variable)"
	echo "      when executing this script."
	echo ""
	echo "Note: The tests launch up to four processes. Make sure the MPI and SLURM environment is ready for"
	echo "      launching four processes."
	echo ""
	if [ "$#" -eq 1 ]; then
		exit 1
	fi
}

function find_binary {
	found=0
	for binary in "$@"; do
		which $binary &> /dev/null
		if [ $? -eq 0 ]; then
			found=$binary
			break
		fi
	done

	echo $found
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

makefile=./Makefile
if [ ! -f $makefile ]; then
	usage "Makefiles not found! Execute this script from the './tampi/tests' folder"
fi

red="\033[0;31m"
green="\033[0;32m"
clean="\033[0m"

# Default parameters
large_input=0
use_slurm=0
tampi_path=
tampi_inc_path=
tampi_lib_path=
skip_omp=0
keep_bins=0

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
	elif [ "$opt" == "--slurm" ]; then
		use_slurm=1
		continue
	elif [ "$opt" == "--skip-omp" ]; then
		skip_omp=1
		continue
	elif [ "$opt" == "--keep" ]; then
		keep_bins=1
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

# MPI/Slurm Commands
mpicxx=$(find_binary mpiicpc mpicxx)
mpif90=$(find_binary mpiifort mpif90)

nnodes=
nprocs=4
ncpusxproc=

if [ $use_slurm -eq 1 ]; then
	launcher=$(find_binary srun)
	if [ -z "$SLURM_JOB_NUM_NODES" ]; then
		usage "SLURM_JOB_NUM_NODES is not defined; ensure there is sbatch/salloc session"
	elif [ -z "$SLURM_NTASKS" ]; then
		usage "SLURM_NTASKS is not defined; ensure --ntasks option was specified"
	elif [ $SLURM_NTASKS -lt $nprocs ]; then
		usage "SLURM_NTASKS is less than $nprocs tasks"
	elif [ -z "$SLURM_CPUS_PER_TASK" ]; then
		usage "SLURM_CPUS_PER_TASK is not defined; ensure --cpus-per-task was specified"
	fi
	nnodes=$SLURM_JOB_NUM_NODES
	ncpusxproc=$SLURM_CPUS_PER_TASK
else
	launcher=$(find_binary mpiexec.hydra mpiexec mpirun)
fi

check_binaries $mpicxx $mpif90 $launcher clang++

echo "---------------------------------------"
echo "TAMPI TEST SUITE"
echo "---------------------------------------"
echo ""
echo "Using TAMPI from:"
echo "  Headers:       $tampi_inc_path"
echo "  Libraries:     $tampi_lib_path"
echo ""
echo "Using MPI from:"
echo "  C++:           $(which $mpicxx)"
echo "  Fortran:       $(which $mpif90)"
echo "  Launcher:      $(which $launcher)"
echo ""
echo "Using OmpSs-2 from:"
echo "  clang++ (C++): $(which clang++)"
echo ""
echo "Using input data:"
if [ $large_input -eq 1 ]; then
	echo "  Large"
else
	echo "  Normal"
fi
echo ""
echo "Configuration:"
echo "  Processes: $nprocs"
if [ $use_slurm -eq 1 ]; then
	echo "  CPUs per process: $ncpusxproc"
	echo "  Nodes: $nnodes"
fi
echo ""

progs=(
	CollectiveBlk.oss.test
	CollectiveNonBlk.omp.test
	CollectiveNonBlk.oss.test
	DoNotExecute.oss.test
	DoNotExecutef.oss.test
	HugeBlkTasks.oss.test
	HugeTasksf.oss.test
	InitAuto.oss.test
	InitAutoTaskAware.oss.test
	InitExplicit.oss.test
	InitExplicitTaskAware.oss.test
	MultiPrimitiveBlk.oss.test
	MultiPrimitiveNonBlk.omp.test
	MultiPrimitiveNonBlk.oss.test
	PrimitiveBlk.oss.test
	PrimitiveNonBlk.omp.test
	PrimitiveNonBlk.oss.test
	PrimitiveWrappers.oss.test
	ThreadDisableTaskAwareness.oss.test
	ThreadTaskAwareness.oss.test
)

echo "Compiling tests..."
compile_args="TAMPI_INCLUDE_PATH=$tampi_inc_path TAMPI_LIBRARY_PATH=$tampi_lib_path MPICXX=$mpicxx MPIF90=$mpif90"
if [ $large_input -eq 1 ]; then
	compile_args="$compile_args LARGE_INPUT=1"
fi

make -f $makefile -B -s ${progs[*]} $compile_args
if [ $? -ne 0 ]; then
	exit 1
fi

nprogs=${#progs[@]}
nfailed=0

echo "---------------------------------------"
echo "TOTAL TESTS: $nprogs"
echo "---------------------------------------"

for ((i=0;i<nprogs;i++)); do
	prog=${progs[$i]}

	echo -n "${prog} "

	if [[ "$prog" == *"DoNotExecute"* ]]; then
		echo -e "${green}SKIPPED${clean}"
		continue
	fi
	if [ $skip_omp -eq 1 ]; then
		if [[ "$prog" == *".omp."* ]]; then
			echo -e "${green}SKIPPED${clean}"
			continue
		fi
	fi

	if [ $use_slurm -eq 1 ]; then
		${launcher} -N ${nnodes} -n ${nprocs} -c ${ncpusxproc} --cpu_bind=cores ./${prog} &> /dev/null
	else
		${launcher} -np ${nprocs} ./${prog} &> /dev/null
	fi

	if [ $? -eq 0 ]; then
		echo -e "${green}PASSED${clean}"
	else
		echo -e "${red}FAILED${clean}"
		nfailed=$(($nfailed + 1))
	fi
done

if [ $keep_bins -eq 0 ]; then
	make -f $makefile -B -s clean $compile_args
	if [ $? -ne 0 ]; then
		exit 1
	fi
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

