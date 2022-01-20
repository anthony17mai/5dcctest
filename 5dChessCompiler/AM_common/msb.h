#pragma once

inline constexpr unsigned int zero_count(unsigned int);

#ifdef __GNUC__

inline constexpr unsigned int zero_count(unsigned int i)
{
	return __bultin_clz(i);
}

#elifdef 

#endif // __GNUC__
