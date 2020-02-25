/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef ARRAY_VIEW_HPP
#define ARRAY_VIEW_HPP

#include <iterator>

namespace util {

template< typename T >
class ArrayView {
private:
	T* _begin;
	T* _end;

public:
	ArrayView(T *begin, int length)
		: _begin(begin),
		_end(begin + static_cast<size_t>(length))
	{
	}

	ArrayView(T *begin, size_t length)
		: _begin(begin),
		_end(begin + length)
	{
	}

	ArrayView(T *begin, T *end)
		: _begin(begin),
		_end(end)
	{
	}

	int size() const
	{
		return std::distance(begin(), end());
	}

	bool empty() const
	{
		return begin() == end();
	}

	T& operator[](size_t pos)
	{
		return _begin[pos];
	}

	T* data()
	{
		return _begin;
	}

	T* begin()
	{
		return _begin;
	}

	T* end()
	{
		return _end;
	}

	const T& operator[](size_t pos) const
	{
		return _begin[pos];
	}

	const T* data() const
	{
		return _begin;
	}

	const T* begin() const
	{
		return _begin;
	}

	const T* end() const
	{
		return _end;
	}

	std::reverse_iterator<T*> rbegin()
	{
		return std::reverse_iterator<T*>(end());
	}

	std::reverse_iterator<T*> rend()
	{
		return std::reverse_iterator<T*>(begin());
	}

	std::reverse_iterator<const T*> rbegin() const
	{
		return std::reverse_iterator<T*>(end());
	}

	std::reverse_iterator<const T*> rend() const
	{
		return std::reverse_iterator<T*>(begin());
	}
};

template < typename T >
struct reverse_adaptor {
	T& iterable;

	auto begin() -> decltype(iterable.rbegin()) { return iterable.rbegin(); }

	auto end() -> decltype(iterable.rend()) { return iterable.rend(); }

	auto rbegin() -> decltype(iterable.begin()) { return iterable.begin(); }

	auto rend() -> decltype(iterable.end()) { return iterable.end(); }
};

template < typename T >
util::reverse_adaptor<T> reverse(T&& iterable) { return {iterable}; }

} // namespace util

#endif // ARRAY_VIEW_HPP

