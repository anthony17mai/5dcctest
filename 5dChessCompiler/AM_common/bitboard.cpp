#include "bitboard.h"
#include "bit_reversal.h"
#include <cmath>
#include "macros.h"

//const int AM_COMMON::bitboard::BYTE_SIZE = 8 * sizeof(byte_t);

void AM_COMMON::bitboard::set_t(integ_t bit_index)
{
	int arr_ind =	arr_index(bit_index);
	int rem =		arr_rem(bit_index);

	arr[arr_ind] = control(rem) | arr[arr_ind];
}

void AM_COMMON::bitboard::set_f(integ_t bit_index)
{
	int arr_ind = arr_index(bit_index);
	int rem = arr_rem(bit_index);

	arr[arr_ind] = ~control(rem) & arr[arr_ind];
}

void AM_COMMON::bitboard::reverse_arr()
{
	//TODO: fix: only works for bitboards with bitcount == 0 mod 8
	integ_t size = arr_size();
	byte_t* new_arr = new byte_t[size];

	//fill the new arr
	AM_COMMON::reverse_arr(this->arr, new_arr, size);

	//replace the arr
	byte_t* tmp = this->arr;
	this->arr = new_arr;

	//delete old arr
	delete[] tmp;
}

AM_COMMON::bitboard& AM_COMMON::bitboard::operator=(const bitboard& other)
{
	//copy the other's array
	integ_t size = arr_size() < other.arr_size() ? arr_size() : other.arr_size();
	//std::copy
	std::copy(other.arr, other.arr + arr_size(), this->arr);

	return *this;
}

AM_COMMON::bitboard::bitboard(const bitboard& other) : width(other.width), height(other.height), arr(new byte_t[other.arr_size()]) 
{
	//std::copy
	std::copy(other.arr, other.arr + arr_size(), this->arr);
}

#include "macros_undef.h"