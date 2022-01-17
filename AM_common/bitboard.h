#pragma once

#include "macros.h"
#include <inttypes.h>
#include <xutility>

namespace AM_COMMON
{
	struct bitboard
	{
		typedef uint8_t byte_t;		//i could choose to change the byte_t to some larger value if i so choose
		typedef size_t	integ_t;	//probably change this to size_t
		const static int BYTE_SIZE = 8 * sizeof(byte_t);	//size of a byte_t

		const integ_t width;
		const integ_t height;
		//use a dynamic array so that i can handle larger boards potentially
		byte_t* arr;

		static inline constexpr integ_t bytesize(integ_t bitsize)			{ return ((bitsize + BYTE_SIZE - 1) / BYTE_SIZE); }
		static inline constexpr integ_t arr_size(integ_t width, integ_t height) { return bytesize(width * height); }
		static inline byte_t control(integ_t remainder)			{ return (byte_t)1 << remainder; }
		static inline integ_t bit_index(integ_t x, integ_t y, integ_t width, integ_t height) { return x + width * y; }		//get the bit index of a coordnate x + width * y
		static inline integ_t arr_index(integ_t num)			{ return num / BYTE_SIZE; }
		static inline integ_t arr_rem(integ_t num)				{ return num % BYTE_SIZE; }

		inline integ_t bit_index(integ_t x, integ_t y) const	{ return bit_index(x, y, width, height); }
		inline integ_t arr_index(integ_t x, integ_t y) const	{ return arr_index(bit_index(x, y, width, height)); }
		inline integ_t arr_size() const							{ return arr_size(width, height); }
		inline byte_t get_byte(integ_t arr_index) const			{ return arr[arr_index]; }
		inline byte_t get_byte(integ_t x, integ_t y) const		{ return get_byte(arr_index(x, y)); }
		inline byte_t get_bit(integ_t bit_index) const			{ return control(bit_index % BYTE_SIZE) & arr[arr_index(bit_index)]; }
		inline byte_t get_bit(integ_t x, integ_t y) const		{ return get_bit(bit_index(x, y)); }
		void set_t(integ_t bit_index);
		void set_f(integ_t bit_index);
		void set(integ_t bit_index, bool val) { if (val) set_t(bit_index); else set_f(bit_index); }
		void set(integ_t x, integ_t y, bool val) { set(bit_index(x, y), val); }
		inline void set_byte(integ_t arr_index, byte_t byte) const { arr[arr_index] = byte; }
		void reverse_arr();			//TODO: fix this

		bitboard& operator=(const bitboard& other);

		//deep copy
		bitboard(const bitboard& other);
		//ctor
		bitboard(integ_t width, integ_t height) : width(width), height(height), arr(new byte_t[arr_size(width, height)]) 
		{
			//initialize each byte to 0
			std::fill(arr, arr + arr_size(width, height), (byte_t)0);
			/*
			for (size_t i = 0; i < arr_size(width, height); i++)
			{
				arr[i] = (byte_t)0;
			}
			*/
		}

		~bitboard() { delete[] arr; }
	};
}