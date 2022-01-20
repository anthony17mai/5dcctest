#pragma once

namespace AM_common
{
	//strictly input terator
	//turn this somehow into a function?
	//TODO: learn metaprogramming to restrict base_iter to iterator types
	/*
	Adapts an iterator by using a function
	*/
	template<typename base_iter, typename adaptor_fctr>
	struct iter_adapter_input
	{
		/*
		underlying iterator object
		*/base_iter iter;

		/*
		starting type
		*/typedef decltype(*iter) ts;
		/*
		extract the resultant type from the adaptor
		*/typedef decltype(adaptor_fctr()(*iter)) _adapt_ty;

		/*
		adapt the value
		*/_adapt_ty operator*() { return adaptor_fctr()(*iter); }
		iter_adapter_input& operator++() { iter++; return *this; }
		iter_adapter_input operator++() { iter_adapter_input tmp = *this; iter++; return tmp; }

		iter_adapter_input(base_iter iter) iter(iter) {}
	};
}