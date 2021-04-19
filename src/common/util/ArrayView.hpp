/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2021 Barcelona Supercomputing Center (BSC)
*/

#ifndef ARRAY_VIEW_HPP
#define ARRAY_VIEW_HPP

#include <cassert>
#include <iterator>


namespace tampi {
namespace util {

//! Class that encapsulates an array previously allocated
//! and that his storage should be kept valid during the
//! lifetime of this object
template <typename T>
class ArrayView {
private:
	//! The begin of the array
	T* _begin;

	//! The end of the array (not included)
	T* _end;

public:
	//! \brief Construct an arrayview
	//!
	//! \param begin A pointer to the first element
	//! \param length The length of the arrayview
	inline ArrayView(T *begin, int length) :
		_begin(begin),
		_end(begin + static_cast<size_t>(length))
	{
		assert(length >= 0);
	}

	//! \brief Construct an arrayview
	//!
	//! \param begin A pointer to the first element
	//! \param length The length of the arrayview
	inline ArrayView(T *begin, size_t length) :
		_begin(begin),
		_end(begin + length)
	{
	}

	//! \brief Get the number of elements in the arrayview
	//!
	//! \returns The number of elements
	inline int size() const
	{
		return std::distance(begin(), end());
	}

	//! \brief Indicate whether the arrayview is empty
	//!
	//! \returns Whether it is empty
	inline bool empty() const
	{
		return begin() == end();
	}

	//! \brief Get a pointer to the first element
	//!
	//! \returns A pointer to the first element
	inline T* begin()
	{
		return _begin;
	}

	//! \brief Get a pointer to the first element
	//!
	//! \returns A pointer to the first element
	inline const T* begin() const
	{
		return _begin;
	}

	//! \brief Get a pointer to the end of the array
	//!
	//! This function returns a pointer to the end of the
	//! array, which is not a valid element. Accessing it
	//! results in undefined behavior
	//!
	//! \returns A pointer to the end
	inline T* end()
	{
		return _end;
	}

	//! \brief Get a pointer to the end of the array
	//!
	//! This function returns a pointer to the end of the
	//! array, which is not a valid element. Accessing it
	//! results in undefined behavior
	//!
	//! \returns A pointer to the end
	inline const T* end() const
	{
		return _end;
	}

	//! \brief Get the elements at a given position
	//!
	//! \param position The position of the element
	//!
	//! \returns The element
	inline T& operator[](size_t position)
	{
		return _begin[position];
	}

	//! \brief Get the elements at a given position
	//!
	//! \param position The position of the element
	//!
	//! \returns The element
	inline const T& operator[](size_t position) const
	{
		return _begin[position];
	}

	//! \brief Get a pointer to the underlying array data
	//!
	//! \returns A pointer to the array data
	inline T* data()
	{
		return _begin;
	}

	//! \brief Get a pointer to the underlying array data
	//!
	//! \returns A pointer to the array data
	inline const T* data() const
	{
		return _begin;
	}

	//! \brief Get a reverse iterator to the begin
	//!
	//! \returns A reverse iterator to the begin
	inline std::reverse_iterator<T*> rbegin()
	{
		return std::reverse_iterator<T*>(end());
	}

	//! \brief Get a reverse iterator to the begin
	//!
	//! \returns A reverse iterator to the begin
	inline std::reverse_iterator<const T*> rbegin() const
	{
		return std::reverse_iterator<T*>(end());
	}

	//! \brief Get a reverse iterator to the end
	//!
	//! \returns A reverse iterator to the end
	inline std::reverse_iterator<T*> rend()
	{
		return std::reverse_iterator<T*>(begin());
	}

	//! \brief Get a reverse iterator to the end
	//!
	//! \returns A reverse iterator to the end
	inline std::reverse_iterator<const T*> rend() const
	{
		return std::reverse_iterator<T*>(begin());
	}
};

template <typename T>
struct reverse_adaptor {
	T& iterable;

	auto begin() -> decltype(iterable.rbegin())
	{
		return iterable.rbegin();
	}

	auto end() -> decltype(iterable.rend())
	{
		return iterable.rend();
	}

	auto rbegin() -> decltype(iterable.begin())
	{
		return iterable.begin();
	}

	auto rend() -> decltype(iterable.end())
	{
		return iterable.end();
	}
};

template <typename T>
util::reverse_adaptor<T> reverse(T&& iterable)
{
	return {iterable};
}

} // namespace util
} // namespace tampi

#endif // ARRAY_VIEW_HPP
