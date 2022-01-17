#pragma once

#include "macros.h"
#include <iostream>
#include <string>

namespace AM_COMMON
{
	bool seek_line(std::istream& stream, const std::string& str);
}