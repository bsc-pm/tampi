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


namespace tampi {

//! Class that represents an environment variable
template <typename T>
class EnvironmentVariable {
private:
	//! The environment variable value
	T _value;

	//! Indicate whether the environment variable is defined
	bool _isPresent;

	//! The environment variable name
	std::string _name;

public:
	//! \brief Construct an environment variable
	//!
	//! \param name The name of the environment variable
	//! \param defaultValue An optional value to assign if the environment variable has not been defined
	EnvironmentVariable(std::string const &name, T defaultValue = T()) :
		_value(defaultValue),
		_name(name)
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

	//! \brief Indicate if the enviornment variable has actually been defined
	//!
	//! \returns Whether the enviornment variable is defined
	inline bool isPresent() const
	{
		return _isPresent;
	}

	//! \brief Retrieve the current value
	//!
	//! \returns The current value
	inline T get() const
	{
		return _value;
	}

	//! \brief Retrieve the current value
	//!
	//! \returns The current value
	operator T() const
	{
		return _value;
	}

	//! \brief Overwrite the value
	//!
	//! This method does not alter the actual enviornment variable, it only
	//! modifies the value stored in the object
	//!
	//! \param value The new value
	//! \param makePresent Mark it as if it had been originally defined
	inline void setValue(T value, bool makePresent = false)
	{
		_value = value;
		_isPresent |= makePresent;
	}
};

} // namespace tampi

#endif // ENVIRONMENT_VARIABLE_HPP
