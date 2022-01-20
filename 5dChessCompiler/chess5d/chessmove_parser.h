#pragma once

#include "game_parse.h"
#include "chessmove.h"
#include <piececode.h>
#include "state5d.h"

namespace game
{
	template<> struct parse_container<chessmove>
	{
		enum class move_ty
		{
			ERROR,
			NORMAL,	
			NORM_JUMP,	// >
			TIME_JUMP, // >>
		};

		//flags
		bool parse_success = false;
		bool implicit_tl = false;
		bool implicit_tl2 = false;

		//metadata
		piececode cp;
		move_ty move_type = move_ty::ERROR;
		
		//components
		coord from, to;
		parse_container<board_id_t> from_board, to_board;
		
		//dynamic semantics
		chessmove construct(const state5d& currentcb, bool white_move)
		{
			if (!this->parse_success) throw -1;

			chessmove cm;
			cm.color = white_move;

			//TODO: parse implicit from coordnate / implicit piececide?
			cm.from = from;
			cm.to = to;

			if (implicit_tl)
			{
				//assert only one timeline
				if (currentcb.num_tl() != 1) throw "parse_error";

				//fist timeline is implicitly 0Tt where t is the last timeline of 0L
				int tnum = currentcb.get_timeline(0)->tail_idx();
				cm.from_board = board_id_t(tnum, 0);
			}
			else
			{
				cm.from_board = from_board.construct(currentcb.is_even());
			}

			if (implicit_tl2)
			{
				//second boardid copies first
				cm.to_board = cm.from_board;
			}
			else
			{
				cm.to_board = to_board.construct(currentcb.is_even());
			}

			return cm;
		}
	};

	//[(nTn)]Pln [(nTn)]ln
	//[(nTn)]Pln > [(nTn)]ln
	//[(nTn)]Pln >> [(nTn)]ln
	template<> struct parser<chessmove>
	{
		parse_container<chessmove> operator()(std::istream & str)
		{
			parse_container<chessmove> container;

			//first coordnate
			//skip whitespace
			while (isspace(str.peek()))
			{
				str.get();
			}

			//parse first boardid
			if (str.peek() == '(')
			{
				//explicit timeline - parse boardid
				auto res = parser<board_id_t>()(str);
				if (!res.parse_success) return container;
				container.from_board = res;
			}
			else
			{
				//implicit timeline
				container.implicit_tl = true;
			}

			//parse piece - technically not used
			auto strpos = str.tellg();
			auto res = parser<piececode, piececode::PARSE_MODE_PGN>()(str);
			if (!res.parse_success)
			{
				//if fail then there is no piece and reset the stream counter
				str.seekg(strpos);
			}
			else
			{
				container.cp = res;
			}

			//parse coordnate
			container.from = parser<coord>()(str);

			//find move type
			//skip whitespace
			while (isspace(str.peek()))
			{
				str.get();
			}
			if (str.peek() == '>')
			{
				str.get();
				if (str.peek() == '>')
				{
					//travel
					container.move_type = parse_container<chessmove>::move_ty::TIME_JUMP;
					str.get();
				}
				else
				{
					//jump
					container.move_type = parse_container<chessmove>::move_ty::NORM_JUMP;
				}
			}
			else
			{
				//norm move
				container.move_type = parse_container<chessmove>::move_ty::NORM_JUMP;
			}

			//to coordnate
			//skip whitespace
			while (isspace(str.peek()))
			{
				str.get();
			}

			//parse boardid
			if (str.peek() == '(')
			{
				//parse a boardid
				auto res = parser<board_id_t>()(str);
				if (!res.parse_success) return container;
				container.to_board = res;
			}
			else
			{
				container.implicit_tl2 = true;
			}

			//parse coordnate
			container.to = parser<coord>()(str);

			//parse success
			container.parse_success = true;
			return container;
		}
	};
}