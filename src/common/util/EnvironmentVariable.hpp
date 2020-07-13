/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2020 Barcelona Supercomputing Center (BSC)
*/

#ifndef ENVIRONMENT_VARIABLE_HPP
#define ENVIRONMENT_VARIABLE_HPP


#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>


class StringifiedBool {
private:
	bool _value;

public:
	StringifiedBool()
		: _value(false)
	{
	}

	StringifiedBool(bool value)
		: _value(value)
	{
	}

	StringifiedBool(StringifiedBool const &other)
		: _value(other._value)
	{
	}

	StringifiedBool& operator=(const StringifiedBool& other) = default;

	operator bool() const
	{
		return _value;
	}
};

class StringifiedMemorySize {
private:
	size_t _value;

public:
	StringifiedMemorySize()
		: _value(0)
	{
	}

	StringifiedMemorySize(size_t value)
		: _value(value)
	{
	}

	StringifiedMemorySize(StringifiedMemorySize const &other)
		: _value(other._value)
	{
	}

	StringifiedMemorySize& operator=(const StringifiedMemorySize& other) = default;

	operator size_t() const
	{
		return _value;
	}
};


//! \brief A class to read environment variables
template <typename T>
class EnvironmentVariable {
private:
	T _value;
	bool _isPresent;
	std::string _name;

public:
	//! \brief constructor
	//!
	//! \param[in] name the name of the environment variable
	//! \param[in] defaultValue an optional value to assign if the environment variable has not been defined
	EnvironmentVariable(std::string const &name, T defaultValue = T())
		: _value(defaultValue), _name(name)
	{
		char const *valueString = getenv(name.c_str());
		if (valueString != nullptr) {
			std::istringstream iss(valueString);
			T assignedValue = defaultValue;
			iss >> assignedValue;
			if (!iss.fail()) {
				_value = assignedValue;
				_isPresent = true;
			} else {
				std::cerr << "Warning: invalid value for environment variable " << name << ". Defaulting to " << defaultValue << "." << std::endl;
				_isPresent = false;
			}
		} else {
			_isPresent = false;
		}
	}

	//! \brief indicates if the enviornment variable has actually been defined
	inline bool isPresent() const
	{
		return _isPresent;
	}

	//! \brief retrieve the current value
	inline T getValue() const
	{
		return _value;
	}

	//! \brief retrieve the current value
	operator T() const
	{
		return _value;
	}

	//! \brief overwrite the value
	//!
	//! Note that this method does not alter the actual enviornment variable. It
	//! only modifies the value stored in the object.
	//!
	//! \param[in] value the new value
	//! \param[in] makePresent mark it as if it had been originally defined
	inline void setValue(T value, bool makePresent = false)
	{
		_value = value;
		_isPresent |= makePresent;
	}

};


template <>
class EnvironmentVariable<StringifiedBool> {
private:
	StringifiedBool _value;
	bool _isPresent;
	std::string _name;

public:
	EnvironmentVariable(std::string const &name, StringifiedBool defaultValue = StringifiedBool())
		: _value(defaultValue), _name(name)
	{
		char const *valueString = getenv(name.c_str());
		if (valueString != nullptr) {
			if (std::string("true") == valueString) {
				_value = true;
				_isPresent = true;
			} else if (std::string("false") == valueString) {
				_value = false;
				_isPresent = true;
			} else {
				std::cerr << "Warning: invalid value for environment variable " << name << ". Defaulting to " << defaultValue << "." << std::endl;
				_isPresent = false;
			}
		} else {
			_isPresent = false;
		}
	}

	inline bool isPresent() const
	{
		return _isPresent;
	}

	inline StringifiedBool getValue() const
	{
		return _value;
	}

	operator StringifiedBool() const
	{
		return _value;
	}

	inline void setValue(StringifiedBool value, bool makePresent = false)
	{
		_value = value;
		_isPresent |= makePresent;
	}

};


template <>
class EnvironmentVariable<StringifiedMemorySize> {
private:
	StringifiedMemorySize _value;
	bool _isPresent;
	std::string _name;

public:
	EnvironmentVariable(std::string const &name, StringifiedMemorySize defaultValue = StringifiedMemorySize())
		: _value(defaultValue), _name(name)
	{
		char const *valueString = getenv(name.c_str());
		if (valueString != nullptr) {
			_value = memparse(valueString);
			_isPresent = true;
		} else {
			_isPresent = false;
		}
	}

	inline bool isPresent() const
	{
		return _isPresent;
	}

	inline StringifiedMemorySize getValue() const
	{
		return _value;
	}

	operator StringifiedMemorySize() const
	{
		return _value;
	}

	inline void setValue(StringifiedMemorySize value, bool makePresent = false)
	{
		_value = value;
		_isPresent |= makePresent;
	}

private:

	/** It parses a string representing a size in the form
	 * 'xxxx[k|K|m|M|g|G|t|T|p|P|e|E]' to a size_t value. */
	inline size_t memparse(const char *ptr)
	{
		char *endptr;

		size_t ret = strtoull(ptr, &endptr, 0);

		switch (*endptr) {
			case 'E':
			case 'e':
				ret <<= 10;
				// fall through
			case 'P':
			case 'p':
				ret <<= 10;
				// fall through
			case 'T':
			case 't':
				ret <<= 10;
				// fall through
			case 'G':
			case 'g':
				ret <<= 10;
				// fall through
			case 'M':
			case 'm':
				ret <<= 10;
				// fall through
			case 'K':
			case 'k':
				ret <<= 10;
				endptr++;
				// fall through
			default:
				break;
		}

		return ret;
	}
};

#endif // ENVIRONMENT_VARIABLE_HPP
