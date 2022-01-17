#include "bit_reversal.h"

#include "macros.h"

namespace AM_COMMON
{
	inline uint8_t* create_byte_table()
	{
		uint8_t* map = new uint8_t[0x100];

		//use int so that int overflow is avoided
		for (int i = 0; i < 0x100; i++)
		{
			uint8_t reverse = 0;
			for (int j = 0; j < 8; j++)
			{
				int msk = 1 << j;

				//get the reverse of the byte
				if (i & msk)
				{
					//add the bit to the reverse
					reverse = reverse | (uint8_t)(1 << (8 - j - 1));
				}
			}

			//add to the array
			map[i] = reverse;
		}

		return map;
	}

	const uint8_t* rtable = create_byte_table();

	inline constexpr uint8_t reverse_byte(uint8_t byte)
	{
		return rtable[byte];
	}

	void reverse_arr(const uint8_t* arr, uint8_t* out, size_t len)
	{
		for (size_t i = 0; i < len; i++)
		{
			size_t other = len - i - 1;

			uint8_t byte = arr[other];
			out[i] = reverse_byte(byte);
		}
	}
}