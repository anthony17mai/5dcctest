#pragma once

#include <utility>
#include <cinttypes>

template<typename... ts>
struct meta_arr {}; 
template<typename t1, typename t2>
struct meta_list {};
template<uint8_t number>
struct meta_num { const static uint8_t num = number; };
template<int number>
struct meta_number { const static uint8_t num = number; };
template<typename T, T val>
struct wrapper { const static T eval = val; };

template<typename ts>
struct increment;
template<int i>
struct increment<meta_number<i>> { using eval = meta_number<i + 1>; };

//nodefault
template<typename T, typename arr, T... vals>
struct wrap_arr_h;
template<typename T, typename... args, T val, T... vals>
struct wrap_arr_h<T, meta_arr<args...>, val, vals...> 
{
	using eval = typename wrap_arr_h<T, meta_arr<args..., wrapper<T, val>>, vals...>::eval;
};
template<typename T, typename arr>
struct wrap_arr_h<T, arr>
{
	using eval = arr;
};

template<typename T, T... vals>
struct wrap_arr { using eval = typename wrap_arr_h<T, meta_arr<>, vals...>::eval; };

template<typename arr>
struct arr_len;
template<typename t0, typename... ts>
struct arr_len<meta_arr<t0, ts...>>
{
	using eval = typename increment<typename arr_len<meta_arr<ts...>>::eval>::eval;
};
template<>
struct arr_len<meta_arr<>>
{
	using eval = meta_number<0>;
};

template<typename list, typename arr = meta_arr<>>
struct listToArr;
template<typename o, typename list, typename... ts>
struct listToArr<meta_list<o, list>, meta_arr<ts...>>
{
	using eval = typename listToArr<list, metaArr<ts..., o>>::eval;
};
template<typename... ts>
struct listToArr<void, meta_arr<ts...>>
{
	using eval = metaArr<ts...>;
};

template<uint8_t lower, uint8_t upper>
struct domain 
{
	using lower_t = domain<meta_num<lower + 1>, meta_num<upper>>;
	using list = meta_list<meta_num<lower>, typename lower_t::list>;
	using eval = typename listToArr<list>::eval;

};
template<uint8_t num>
struct domain<num, num>
{
	using list = meta_list<meta_num<num>, void>;
	using eval = typename listToArr<list>::eval;
};

template<typename ts, typename... types>
struct construct_list 
{
	using eval = meta_list<ts, typename construct_list<types...>::eval>;
};
template<typename ts>
struct construct_list<ts>
{
	using eval = meta_list<ts, void>;
};

//takes a number and reverses it
template<uint8_t num>
struct bit_reversal
{
	const static uint8_t right	= 0x01;

#define mymacro(n1, n2) (num & (right << n1)) << n2

	static const uint8_t b0 = mymacro(0, 7);
	static const uint8_t b1 = mymacro(1, 6);
	static const uint8_t b2 = mymacro(2, 5);
	static const uint8_t b3 = mymacro(3, 4);
	static const uint8_t b4 = mymacro(4, 3);
	static const uint8_t b5 = mymacro(5, 2);
	static const uint8_t b6 = mymacro(6, 1);
	static const uint8_t b7 = mymacro(7, 0);

#undef mymacro
#define mymacro(n) ((b ## n) >> n)

	static const uint8_t eval = mymacro(0) | mymacro(1) | mymacro(2) | mymacro(3) | mymacro(4) | mymacro(5) | mymacro(6) | mymacro(7);
#undef mymacro
};

//creates a pair of 
template<uint8_t num>
struct reversepair
{
	static const std::pair<uint8_t, uint8_t> eval = std::pair<uint8_t, uint8_t>(num, bit_reversal<num>::eval);
};

template<typename T, T... vals>
struct static_arr;
template<typename T, T val, T... vals>
struct static_arr<T, val, vals...>
{
	using wrapped = typename wrap_arr<T, val, vals...>::eval;
	using metasize = typename arr_len<typename wrap_arr<T, val, vals...>::eval>::eval;
	using lower_t = static_arr<T, vals...>;

	const static int size = metasize::num;

	const T currentvalue;
	const lower_t lower;

	const T* get_as_arr() { return &currentvalue; }

	//ctor
	static_arr() : currentvalue(val), lower() {}
};

template<typename arr, typename out_rpairarr = static_arr<uint8_t>>
struct arr_to_pairarr;
template<typename ts, typename... t1, uint8_t... t2>
struct arr_to_pairarr<meta_arr<ts, t1...>, static_arr<uint8_t, t2...>>
{
	using num = ts;
	using rpair = reversepair<ts::num>;
	using lower_t = arr_to_pairarr<meta_arr<t1...>, static_arr<uint8_t, (std::pair<uint8_t, uint8_t>)rpair::eval, t2...>>;
	using eval = typename lower_t::eval;
};
template<uint8_t... t>
struct arr_to_pairarr<meta_arr<>, static_arr<uint8_t, t...>>
{
	//type safe return as opposed to unsafe typename return
	using eval = static_arr<uint8_t, t...>;
};

auto a = arr_to_pairarr<meta_arr<meta_num<3>, meta_num<5>>>::rpair::eval;
using ts = arr_to_pairarr<meta_arr<meta_num<3>, meta_num<5>>>::lower_t;