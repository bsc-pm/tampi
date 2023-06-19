/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2023-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef STRING_SUPPORT_HPP
#define STRING_SUPPORT_HPP

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>


namespace tampi {

//! Class providing string parsing functionalities
class StringSupport {
public:
	//! \brief Split a string of elements into a vector of strings
	//!
	//! \param str The string containing elements
	//! \param delim The character delimiter between elements
	//!
	//! \returns The vector of strings
	static std::vector<std::string> split(const std::string &str, char delim = ':')
	{
		std::stringstream stream(str);
		std::vector<std::string> components;

		std::string component;
		while (std::getline(stream, component, delim))
			if (!component.empty())
				components.push_back(component);

		return components;
	}

	//! \brief Parse a string to another type
	//!
	//! \param str The string to parse
	//! \param value The resulting value after parsing
	//!
	//! \returns Whether the parsing succeeded
	template <typename T>
	static bool parse(const std::string &str, T &value);

	//! \brief Parse string containing two elements
	//!
	//! \param str The string to parse
	//! \param value1 The value of the first parsed element
	//! \param value2 The value of the second parsed element
	//!
	//! \returns Whether the parsing succeeded
	template <typename T1, typename T2>
	static bool parse(const std::string &str, bool present[2], T1 &value1, T2 &value2, char delim = ":")
	{
		size_t expected = std::count(str.begin(), str.end(), delim) + 1;
		auto components = split(str, delim);

		if (components.size() != expected || components.size() > 2)
			return false;

		std::fill(present, present + 2, false);

		if (components.size() > 0) {
			present[0] = parse(components[0], value1);
			if (!present[0])
				return false;
		}
		if (components.size() > 1) {
			present[1] = parse(components[1], value2);
			if (!present[1])
				return false;
		}
		return true;
	}

	//! \brief Parse string containing three elements
	//!
	//! \param str The string to parse
	//! \param value1 The value of the first parsed element
	//! \param value2 The value of the second parsed element
	//! \param value3 The value of the third parsed element
	//!
	//! \returns Whether the parsing succeeded
	template <typename T1, typename T2, typename T3>
	static bool parse(const std::string &str, bool present[3], T1 &value1, T2 &value2, T3 &value3, char delim = ":")
	{
		size_t expected = std::count(str.begin(), str.end(), delim) + 1;
		auto components = split(str, delim);

		if (components.size() != expected || components.size() > 3)
			return false;

		std::fill(present, present + 3, false);

		if (components.size() > 0) {
			present[0] = parse(components[0], value1);
			if (!present[0])
				return false;
		}
		if (components.size() > 1) {
			present[1] = parse(components[1], value2);
			if (!present[1])
				return false;
		}
		if (components.size() > 2) {
			present[2] = parse(components[2], value3);
			if (!present[2])
				return false;
		}
		return true;
	}

	//! \brief Retrieve the indices included in a comma-separated list with ranges
	//!
	//! The list of indices is comma-separated and can contain index ranges that
	//! are represented with hypens. For instance, "0,2,5-9,11" would be a valid
	//! list of indices
	//!
	//! \param list The string containing a list of indices and ranges
	//! \param indices The vector of indices that belong to the list
	static inline bool parseIndices(
		const std::string &list, std::vector<uint64_t> &indices
	) {
		std::vector<std::string> ranges = split(list, ',');
		if (ranges.empty())
			return true;

		for (const std::string &range : ranges) {
			if (range.empty())
				continue;

			uint64_t begin, end;
			if (range.find('-') == std::string::npos) {
				// Parse directly if it is not a range
				if (!parse(range, begin))
					return false;
				indices.push_back(begin);
			} else {
				std::vector<std::string> rangeIndices = split(range, '-');
				if (rangeIndices.size() != 2)
					return false;
				if (!parse(rangeIndices[0], begin) || !parse(rangeIndices[1], end))
					return false;
				if (begin > end)
					return false;
				for (uint64_t i = begin; i <= end; ++i)
					indices.push_back(i);
			}
		}
		return true;
	}
};

template <typename T>
static bool StringSupport::parse<T>(const std::string &str, T &value)
{
	std::istringstream stream(str);
	T tmp;
	stream >> tmp;

	if (stream.fail())
		return false;

	value = tmp;
	return true;
}

template <>
inline bool StringSupport::parse<std::string>(const std::string &str, std::string &value)
{
	value = str;
	return true;
}

} // namespace tampi

#endif // STRING_SUPPORT_HPP
