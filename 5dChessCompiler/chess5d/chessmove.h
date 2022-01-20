#pragma once
#include <ctype.h>
#include <cinttypes>
#include "str_split.h"
#include <string>
#include "coord5d.h"
#include "game_parse.h"

namespace game
{
	//replaced with game_parse
	/*
	coord parse_coord(char*& ptr)
	{
		ptr += 2;

		//assume perfect input
		char x = tolower(ptr[0]);	//letter
		char y = tolower(ptr[1]);	//number

		return coord(x - 'a', y - '1');
	}
	*/

	//two ordered pairs
	struct chessmove
	{
		coord from;
		board_id_t from_board;
		coord to;
		board_id_t to_board;
		bool color = true;

		coord5d from_coord() const { return coord5d(from, from_board, color); }
		coord5d to_coord() const { return coord5d(to, to_board, color); }

		bool operator==(const chessmove& o) const 
		{
			return from == o.from
				&& to == o.to
				&& from_board == o.from_board
				&& to_board == o.to_board
				&& color == o.color;
		};
	};
	//parser is located in a seperate file

	//template<> struct parse_container<chessmove> : public default_container<chessmove>
	//[(nTn)]Pln [(nTn)]Pln
	//template<> struct parser<chessmove>
}