#pragma once
#include <type_traits>
#include <limits>

//unfinished

template<typename integral_ty, typename _shamt = std::integral_constant<int, 4 * sizeof(integral_ty)>, typename _mask = std::integral_constant<integral_ty, std::numeric_limits<integral_ty>::max()>, bool _is_unsigned = std::is_unsigned<integral_ty>::value, bool _f = false>
struct bin_bitmask;

template<typename integral_ty, typename _shamt, typename _mask>
struct bin_bitmask<integral_ty, _shamt, _mask, false, false>
{
public:
	using _this_mask = std::integral_constant<integral_ty, (_mask::value >> _shamt::value) | (_mask::value << 2 * _shamt::value)>;
	using _next_t = bin_bitmask<integral_ty, std::integral_constant<integral_ty, _shamt::value / 2>, _this_mask, false, _shamt::value / 2 == 1>;

public:
	inline constexpr integral_ty operator()()
	{

	}
};

//bin_bitmask<unsigned int>;
using test = std::enable_if<std::is_unsigned<unsigned int>::value>;

//using isSigned = std::is_signed<unsigned>;
//using en = std::enable_if<isSigned::value>::type;
//using test = std::void_t<isSigned>;