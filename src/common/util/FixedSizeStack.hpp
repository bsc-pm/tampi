/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef FIXED_SIZE_STACK_HPP
#define FIXED_SIZE_STACK_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>


namespace tampi {

//! Class that implements a stack with a fixed capacity
template <typename T, size_t Capacity>
class FixedSizeStack {
private:
	//! The array of elements
	std::array<T, Capacity> _array;

	//! The current size of the stack
	size_t _size;

public:
	//! \brief Constructs an empty stack
	FixedSizeStack() :
		_array(),
		_size(0)
	{
	}

	//! \brief Checks whether the stack is empty
	bool empty() const
	{
		return _size == 0;
	}

	//! \brief Gets the stack size
	size_t size() const
	{
		return _size;
	}

	//! \brief Gets the stack capacity
	constexpr size_t capacity() const
	{
		return Capacity;
	}

	//! \brief Pushes an element to the stack
	//!
	//! \param element The element to push
	void push(const T &element)
	{
		assert(_size < Capacity);
		_array[_size] = element;
		++_size;
	}

	//! \brief Pushes multiple elements to the stack
	//!
	//! The operation assumes there is enough space to push all the elements
	//!
	//! \param elements The pointer to the elements to push
	//! \param n The number of elements to push
	void push(const T *elements, size_t n)
	{
		assert(_size + n <= Capacity);
		std::copy(elements, elements + n, _array.begin() + _size);
		_size += n;
	}

	//! \brief Pops an element from the stack
	//!
	//! The operation assumes there is at least an element in the stack
	//!
	//! \param element The place where to store the retrieved element
	bool pop(T &element)
	{
		if (_size == 0)
			return false;

		--_size;
		element = _array[_size];
		return true;
	}
};

} // namespace tampi

#endif // FIXED_SIZE_STACK_HPP
