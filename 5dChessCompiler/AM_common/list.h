#pragma once

#include "macros.h"

namespace AM_COMMON
{
	//crappy assign arbitrary elements to array
	template<typename AR_T, typename T>
	void assignArr(AR_T* arr, T t)
	{
		(*arr) = (AR_T)t;
	}

	template<typename AR_T, typename T1, typename... T>
	void assignArr(AR_T* arr, T1 t, T... elements)
	{
		(*arr) = (AR_T)t;
		assignArr(arr + 1, elements...);
	}

	//crappy sized array with length
	template<typename T>
	struct list
	{
		T** arr;
		const int count;

		template<typename... Ts>
		list(Ts*... t) : count(sizeof...(Ts))
		{
			arr = new T*[sizeof...(Ts)];
			assignArr(arr, t...);
		}

		~list()
		{
			//delete the array
			//this removes any memory leak
			for (int i = 0; i < count; i++)
			{
				delete arr[i];
			}
			delete[] arr;
		}
	};

	//crappy list with set template length
	template<typename T, int len>
	struct list_static
	{
		static const int length = len;
		T* arr;

		list_static()
		{
			arr = new T[len];
		}
	};
}
