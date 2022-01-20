#pragma once

#include <type_traits>

//what an iterator should look like in general - not functional code
template<typename _val_ty>
struct iter
{
	using iter = iter;
	using _val_ty = _val_ty;

	using value_type = _val_ty;
	using difference_type = ptrdiff_t;
	using reference = value_type&;
	using pointer = value_type*;
	using iterator_category = std::random_access_iterator_tag;
	_val_ty* ptr;

	//iter
	inline iter operator++(int);
	inline iter& operator++();
	inline iter operator--(int);
	inline iter& operator--();
	inline reference operator[](size_t n) { return ptr[n]; }
	inline reference operator*() { return *ptr; }
	inline difference_type operator-(box_iter o)
	{
		return (ptr - o.ptr);
	}

	box_iter(_val_ty* ptr) : ptr(ptr) {}
};