#pragma once

namespace AM_common {

	//provides bitwise operators while maintaining type safety
	template<typename impl>
	struct flag
	{
		using integ = typename std::underlying_type<impl>::type;

		integ integral;

		flag operator|(flag flagr) const
		{
			return flag(integral | flagr.integral);
		}
		flag operator&(flag flagr) const
		{
			return flag(integral & flagr.integral);
		}
		flag operator^(flag flagr) const
		{
			return flag(integral ^ flagr.integral);
		}

		flag& operator|=(flag flagr)
		{
			integral |= flagr.integral;
			return *this;
		}
		flag& operator&=(flag flagr)
		{
			integral &= flagr.integral;
			return *this;
		}
		flag& operator^=(flag flagr)
		{
			integral ^= flagr.integral;
			return *this;
		}

		flag operator~() { return flag(~integral); }

		bool operator==(integ o) const { return integral == o; }
		bool operator==(impl o) const { return (impl)integral == o; }
		bool operator==(flag o) const { return integral == o.integral; }

		//converter
		operator impl() const
		{
			return (impl)integral;
		}
		operator bool() const
		{
			return (bool)integral;
		}
		flag() = default;
		flag(impl o) : integral((integ)o) {}
		explicit operator integ()
		{
			return integral;
		}
		explicit flag(integ o) : integral(o) {}
	};

	template<typename impl>
	flag<impl> make_flag(impl integ) { return (flag<impl>)integ; }
}