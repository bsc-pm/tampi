/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2024 Barcelona Supercomputing Center (BSC)
*/

#include "Allocator.hpp"
#include "Declarations.hpp"
#include "Operation.hpp"


namespace tampi {

void Allocator::initialize()
{
	ObjAllocator<Operation<C>>::_instance =
		new ObjAllocator<Operation<C>>(OperationCapacity);
	ObjAllocator<CollOperation<C>>::_instance =
		new ObjAllocator<CollOperation<C>>(CollOperationCapacity);
}

void Allocator::finalize()
{
	delete ObjAllocator<Operation<C>>::_instance;
	delete ObjAllocator<CollOperation<C>>::_instance;
	ObjAllocator<Operation<C>>::_instance = nullptr;
	ObjAllocator<CollOperation<C>>::_instance = nullptr;
}

} // namespace tampi
