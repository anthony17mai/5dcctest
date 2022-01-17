#include "seek_line.h"
#include <iostream>

bool AM_COMMON::seek_line(std::istream& stream, const std::string& str)
{
	std::string line;
	while (stream)
	{
		std::getline(stream, line);

		//found line return
		if (line == str)
			return true;
	}

	//did not find line return
	return false;
}
