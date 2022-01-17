#pragma once

#include <type_traits>
#include <xutility>

//box array: an array containing an arbirary number of dimensions.
namespace AM_common
{
	template<typename t1, typename t2>
	struct m_concatinate;
	template<typename tarr, typename t>
	struct m_push_front;
	template<typename tarr, typename t>
	struct m_push_back;

	template<typename t>
	struct m_stopper { using eval = t; };

	//should have its own file / put it in a different project for metaprogramming
	template<typename integ, integ... ns>
	struct m_i_arr;

	template<typename integ, integ i, integ... ns>
	struct m_i_arr<integ, i, ns...>
	{
		using eval = std::integral_constant<integ, i>;
		using lower = m_i_arr<integ, ns...>;
	};
	template<typename integ, integ i>
	struct m_i_arr<integ, i>
	{
		using eval = std::integral_constant<integ, i>;
		using lower = m_stopper<integ>;	//stopper - better than void because it is able to pass a typename
	};

	template<typename arr, size_t i>
	struct m_get
	{
		using eval = typename m_get<typename arr::lower, i - 1>::eval;
	};
	template<typename arr>
	struct m_get<arr, 0>
	{
		using eval = typename arr::eval;
		using lower = typename arr::lower;
	};

	template<typename arr>
	struct m_prod
	{
		using lower = m_prod<typename arr::lower>;
		using eval = std::integral_constant<typename arr::eval::value_type, arr::eval::value * lower::eval::value>;
	};
	template<typename integ>
	struct m_prod<m_stopper<integ>>
	{
		using eval = std::integral_constant<integ, 1>;
	};

	template<typename _val_ty, typename _m_arr>
	struct box_iter
	{
		using dim_arr = _m_arr;
		using lower = box_iter<_val_ty, typename _m_arr::lower>;
		/*
		should increment by this amount
		*/using increment = typename m_prod<typename _m_arr::lower>::eval;

		//specifically the iterator type which is the lower iter
		using value_type = lower;
		using difference_type = ptrdiff_t;	//i cant just subtract two ptrs from each other i also have to divide
		using reference = value_type&;
		using pointer = value_type*;
		using iterator_category = std::random_access_iterator_tag;
		_val_ty* ptr;

		//iter
		inline box_iter operator++(int)
		{
			auto tmp = *this;
			ptr += increment::value;
			return *this;
		}
		inline box_iter& operator++()
		{
			ptr += increment::value;
			return *this;
		}
		inline box_iter operator--(int)
		{
			auto tmp = *this;
			ptr -= increment::value;
			return *this;			
		}
		inline box_iter& operator--()
		{
			ptr -= increment::value;
			return *this;
		}
		inline lower operator[](size_t n) { return lower(ptr + n * increment::value); }
		inline lower operator*() { return lower(ptr); }
		/*
		since these are just wrapped ptrs anyways, I dont need to do a const ref
		*/inline ptrdiff_t operator-(box_iter o)	
		{
			return (ptr - o.ptr) / increment::value;
		}

		//iterates through the element of this sub box
		inline lower begin() { return lower(ptr); }
		inline lower end() { return lower(ptr + increment::value); }

		box_iter(_val_ty* ptr) : ptr(ptr) {}
	};
	template<typename _val_ty, typename integ, integ i>
	struct box_iter<_val_ty, m_i_arr<integ, i>>
	{
		//finally we end the recursion
		using lower = _val_ty;
		using increment = std::integral_constant<integ, 1>;

		//specifically the iterator type which is the lower iter
		using value_type = _val_ty;
		using difference_type = ptrdiff_t;	//i cant just subtract two ptrs from each other i also have to divide
		using reference = value_type&;
		using pointer = value_type*;
		using iterator_category = std::random_access_iterator_tag;
		_val_ty* ptr;

		//iter
		inline box_iter operator++(int)
		{
			auto tmp = *this;
			ptr += increment::value;
			return *this;
		}
		inline box_iter& operator++()
		{
			ptr += increment::value;
			return *this;
		}
		inline box_iter operator--(int)
		{
			auto tmp = *this;
			ptr -= increment::value;
			return *this;
		}
		inline box_iter& operator--()
		{
			ptr -= increment::value;
			return *this;
		}
		inline reference operator[](size_t n) { return ptr[n]; }
		inline reference operator*() { return *ptr; }
		/*
		since these are just wrapped ptrs anyways, I dont need to do a const ref
		*/
		inline ptrdiff_t operator-(box_iter o)
		{
			return (ptr - o.ptr);
		}

		//iterates through the element of this sub box
		inline lower begin() { return ptr; }
		inline lower end() { return ptr + increment::value; }

		box_iter(_val_ty* ptr) : ptr(ptr) {}
	};

	template<typename integ, integ n, integ... ns>
	struct m_push_front<m_i_arr<integ, ns...>, std::integral_constant<integ, n>>
	{
		using eval = m_i_arr<integ, n, ns...>;
	}; 
	template<typename integ, integ n, integ... ns>
	struct m_push_back<m_i_arr<integ, ns...>, std::integral_constant<integ, n>>
	{
		using eval = m_i_arr<integ, n, ns...>;
	};

	//todo: allow random index order using template which is HARD
	//a statically sized multidimensional array with iterators no bounds checking (although checking bounds would be pretty easy)
	template<typename _val_ty, size_t... dims>
	class box_arr
	{
	public:
		using dim_arr = m_i_arr<size_t, dims...>;
		using arr_size = typename m_prod<dim_arr>::eval;
		using iterator = box_iter<_val_ty, dim_arr>;

	private:
		_val_ty _arr[arr_size::value];

	public:
		iterator begin() { return iterator(_arr); }
		iterator end() { return iterator(_arr + arr_size::value); }
		typename iterator::value_type operator[](size_t n) { return begin()[n]; }
	};
}
