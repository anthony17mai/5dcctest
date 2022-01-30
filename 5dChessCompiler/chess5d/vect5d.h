#pragma once
#include "coord5d.h"

namespace game {

	//represents the difference between two coord5ds
	struct vect5d : public AM_common::util::npmk_pair<coord, board_id_t, int>
	{
		using AM_common::util::npmk_pair<coord, board_id_t, int>::npmk_pair;
	};
	coord5d operator+(const coord5d& l, const vect5d& r)
	{
		return coord5d(l._coord + r.first, l._board_id + r.second, l.is_white_board);
	}
	coord5d operator-(const coord5d& l, const vect5d& r)
	{
		return l + (-r);
	}
}

template<> struct std::hash<game::vect5d>
{
	inline size_t operator()(const game::vect5d& vect) 
	{
		return std::hash<game::coord>()(vect.first) ^ ((2 * sizeof(size_t) * std::hash<game::board_id_t>()(vect.second)));
	}
};