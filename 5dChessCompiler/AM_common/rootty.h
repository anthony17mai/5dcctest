#pragma once

namespace AM_common
{
	namespace meta
	{
		template<typename t>
		struct rootty { using eval = t; };
		template<typename t>
		struct rootty<t*> { using eval = typename rootty<t>::eval; };
		template<typename t>
		struct rootty<t&> { using eval = typename rootty<t>::eval; };
		template<typename t>
		struct rootty<t&&> { using eval = typename rootty<t>::eval; };

		template<typename t>
		struct unref { using eval = t; };
		template<typename t>
		struct unref<t&> { using eval = t; };
		template<typename t>
		struct unref<t&&> { using eval = t; };

		using pointered = int*****&&;
		using unpointered = rootty<pointered>::eval;
	}
}