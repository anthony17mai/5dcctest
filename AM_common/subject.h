#pragma once

#include "observer.h"

namespace AM_common
{
	//observable where an observer is allowed to call remove() while executing
	//still not thread safe
	//not allowed to invoke the 
	template<typename... Ts>
	class observable_VR : observable<Ts...>
	{

	};
}