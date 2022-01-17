#pragma once
#include <type_traits>

namespace meta
{
	template<typename... ts>
	struct meta_arr {};

	template<typename arr, typename ts>
	struct arr_push;
	template<typename... t1, typename ty>
	struct arr_push<meta_arr<t1...>, ty> { using eval = meta_arr<t1..., ts>; };

	template<typename arr1, typename arr2>
	struct arr_concat;
	template<typename... t1, typename... t2>
	struct arr_concat<meta_arr<t1...>, meta_arr<t2...>> { using eval = meta_arr<t1..., t2...>; };

	template<typename ts, int i, typename arr = meta_arr<>>
	struct arr_repeat;
	template<typename ts, int i, typename... t1>
	struct arr_repeat<ts, i, meta_arr<t1...>> { using eval = typename arr_push<typename arr_repeat<ts, i-1>::eval, ts>::eval; };
	template<typename ts, typename arr>
	struct arr_repeat<ts, 0, arr> { using eval = arr; };

	template<typename arr, typename t>					struct arr_contains;
	template<typename t, typename... ts>				struct arr_contains<meta_arr<t, ts...>, t> { using eval = std::true_type; };
	template<typename t1, typename t2, typename... ts>	struct arr_contains<meta_arr<t1, ts...>, t2> { using eval = typename arr_contains<meta_arr<ts...>, t2>::eval; };
	template<typename t>								struct arr_contains<meta_arr<>, t> { using eval = std::false_type; };

	template<typename arr, typename t>		struct _unite;
	template<typename t, typename... ts>	struct _unite<meta_arr<ts...>, t> { using eval = typename std::conditional<typename arr_contains<meta_arr<ts...>, t>::eval()(), meta_arr<ts...>, meta_arr<t, ts...>>::type; };

	template<typename arr1, typename arr2, typename _out = meta_arr<>>			struct arr_union { using eval = _out; };
	template<typename t, typename... ts1, typename... ts2, typename... ts3>		struct arr_union<meta_arr<t, ts1...>, arr_union<ts2...>, meta_arr<ts3...>> { using eval = typename arr_union<meta_arr<ts1...>, meta_arr<ts2...>, typename _unite<meta_arr<ts3...>, t>::eval >::eval; };
	template<typename t, typename... ts1, typename... ts2>						struct arr_union<meta_arr<>, meta_arr<t, ts1...>, meta_arr<ts2...>> { using eval = typename arr_union<meta_arr<>, meta_arr<ts1...>, typename _unite<meta_arr<ts2...>, t>::eval>::eval; };
	template<typename... ts>													struct arr_union<meta_arr<>, meta_arr<>, meta_arr<ts...>> { using eval = meta_arr<ts...>; };
}