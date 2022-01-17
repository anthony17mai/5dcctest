#pragma once

namespace AM_common
{
	template<typename ts, size_t arrlen>
	void inline arr_cp(ts* from, ts* to)
	{
		for (size_t i = 0; i < arrlen; i++)
		{
			to[i] = from[i];
		}
	}

	template<typename ts>
	void inline arr_cp(ts* from, ts* to, size_t arrlen)
	{
		for (size_t i = 0; i < arrlen; i++)
		{
			to[i] = from[i];
		}
	}

	template<typename ts, size_t from1_len, size_t from2_len>
	void inline arr_catenate(ts* from1, ts* from2, ts* to)
	{
		arr_cp<ts, from1_len>(from1, to);
		arr_cp<ts, from2_len>(from2, to + from1_len);
	}

	template<typename ts>
	void inline arr_catenate(ts* from1, ts* from2, ts* to, size_t from1_len, size_t from2_len)
	{
		arr_cp<ts>(from1, to, from1_len);
		arr_cp<ts>(from2, to + from1_len, from2_len);
	}
}