#pragma once
#include "vect5d.h"
#include "abstract_iter.h"
#include <unordered_set>
#include <memory>
#include <vector>

namespace game
{
	namespace moveset
	{
		class ab_move_set
		{
		public:
			using iterator = AM_common::abstract_fw_iter<vect5d, ptrdiff_t, true>;

			virtual iterator begin() const = 0;
			virtual iterator end() const = 0;
		};

		class move_set : public ab_move_set
		{
		public:
			using native_iterator = std::unordered_set<vect5d>::iterator;
			using iterator_base = AM_common::abstract_fw_iter_wrapper<native_iterator>;
			using iterator = ab_move_set::iterator;
			
			std::unordered_set<vect5d> moves;

			iterator begin() const override { return iterator(new iterator_base(moves.begin())); }
			iterator end() const override { return iterator(new iterator_base(moves.end())); }
		};
	}
}