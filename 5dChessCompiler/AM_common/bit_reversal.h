#pragma once

#include "macros.h"
#include <map>
#include <cinttypes>

namespace AM_COMMON
{
	extern const uint8_t* rtable;

	inline constexpr uint8_t reverse_byte(uint8_t);
	void reverse_arr(const uint8_t* arr, uint8_t* out, size_t len);
}

#include "macros_undef.h"