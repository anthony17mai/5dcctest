#pragma once

#include <vector>
#include <string>

//TODO: DO REWORK

namespace AM_common
{
	//splits using delim
	//skips empties
	//new strings as opposed to shallow coppies - which somtimes might be preferable
	void str_split(const std::string& str, std::vector<std::string>& out, char delim = ' ');

	//finds the first occurance of delim and splits the string excluding the delimeter
	void str_divide(const std::string& str, std::vector<std::string>& out, char delim = ' ');

	std::string str_trim(const std::string& str, char delim = ' ');
}