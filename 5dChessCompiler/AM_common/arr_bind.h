#pragma once

#include <functional>

namespace meta
{
	template <typename ts, int num, typename fn_t>
	struct arr_bind
	{
		using lower_t = arr_bind<ts, num - 1, std::binder1st<fn_t>>;
		using eval_t = typename lower_t::eval_t;
		static eval_t inline eval(const ts arr[num], fn_t fn)
		{
			auto bind = std::bind1st<fn_t, t>(fn, arr[0]);
			return lower_t::eval(arr + 1, bind);
		}
	};

	template<typename ts, typename fn_t>
	struct arr_bind<ts, 1, fn_t>
	{
		using eval_t = std::binder1st<fn_t>;
		static eval_t inline eval(ts arr[1])
		{
			return std::bind1st<fn_t, ts>(fn, arr[0]);
		}
	};
}