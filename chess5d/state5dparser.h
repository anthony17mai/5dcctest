#pragma once

#include "state5d.h"
#include "many_chessmove.h"
#include <limits>

namespace game
{
	struct pgn_attr
	{
		std::string _str;
		std::string name() const {
			std::vector<std::string> div;
			AM_common::str_divide(_str, div);
			return div.front();
		}
		std::string value() const
		{
			std::vector<std::string> div;
			AM_common::str_divide(_str, div);
			return div[1];
		}

		pgn_attr() : _str() {}
		pgn_attr(std::string value) : _str(value) {}
	};
	template<> struct parser<pgn_attr>
	{
		parse_container<pgn_attr> operator()(std::istream& str)
		{
			std::string name;
			std::string val;
			if (str.peek() != '[') return false;

			str.get();
			std::getline(str, val, ']');
			return pgn_attr(val);
		}
	};

	template<> struct parse_container<state5d, state5d::PARSE_MODE_PGN5D> : public default_container<state5d>
	{
		std::vector<pgn_attr> attrs;
	};

	template<> struct parser<state5d, state5d::PARSE_MODE_PGN5D>
	{
		parse_container<state5d, state5d::PARSE_MODE_PGN5D> operator()(std::istream& str) {
			parse_container<state5d, state5d::PARSE_MODE_PGN5D> out;
			parse_container<pgn_attr> attr;
			std::vector<parse_container<chessboard, chessboard::PARSE_MODE_JSON>> initial_boards;
			bool custom_board = false;
			state5d state;
			std::vector<std::string> tl_labels;

			while (attr = parser<pgn_attr>()(str))
			{
				//if attribute does not contain a space then it is a chessboard
				if (std::find(attr.ob._str.begin(), attr.ob._str.end(), ' ') == attr.ob._str.end())
				{
					//parse chessboards and add them to the initialboards
					std::stringstream str_str(attr.ob._str);
					auto container = parser<chessboard, chessboard::PARSE_MODE_JSON>()(str_str);
					if (container.parse_success)
						initial_boards.push_back(container);
				}
				else if (attr.ob.name() == "InitialMultiverses")
				{
					//parse the initial multiverses
					AM_common::str_split(AM_common::str_trim(attr.ob.value(), '"'), tl_labels, ' ');
				}
				//dont know if i should use case free comparison here
				else if (attr.ob.name() == "Board")
				{
					custom_board = (attr.ob.value() == "\"custom\"");
				}

				out.attrs.push_back(std::move(attr));

				std::string _s;
				std::getline(str, _s);
			}

			//check if even or not
			if (std::find(tl_labels.begin(), tl_labels.end(), std::string("-0")) != tl_labels.end())
			{
				state._is_even = true;
			}
			else
			{
				state._is_even = false;
			}

			//initialize the state
			if (custom_board)
			{
				//TODO: check that board size matches
				//initialize the state with the initial boards
				for (auto& cb : initial_boards)
				{
					board_id_t id = cb.board_id.construct(state.is_even());
					chessboard* ptr = new chessboard(std::move(cb.cb));
					state._board_list.expand(id.second);
					state._board_list.at(id.second)->expand(id.first);
					state._board_list.at(id.second)->at(id.first)->at(cb.white) = std::unique_ptr<chessboard>(ptr);
				}
			}
			else
			{
				//look up the 5d state
				std::string board_name = std::find_if(out.attrs.begin(), out.attrs.end(), [](const pgn_attr& attr) { return attr.name() == "Board"; })->value();
				board_name = AM_common::str_trim(board_name, '\"');
				state = state5d::lookup(board_name);

				if (state.is_empty()) return out;
			}

			//parse the chessmoves
			while (str.good())
			{
				std::string line;
				std::getline(str, line);
				parse_container<many_chessmove> move_list = parse<many_chessmove>(line);
				if (!move_list.parse_success) return out;

				auto chessmoves = move_list.construct(state);

				for (auto& cm : chessmoves.moves_white)
				{
					bool b = state.move(cm);
					if (!b) return out;
				}

				for (auto& cm : chessmoves.moves_black)
				{
					bool b = state.move(cm);
					if (!b) return out;
				}
			}

			out.parse_success = (state._board_list.size() > 0);
			out.ob = std::move(state);	//always move last!
			return out;
		}
	};

	template<> struct parser<state5d, state5d::PARSE_MODE_JSON>
	{
		state5d operator()(std::istream& str) {
			nlohmann::json ob;
			str >> ob;
			return (state5d)ob;
		}
	};

	template<> struct writer<state5d, state5d::PARSE_MODE_JSON>
	{
		void operator()(std::ostream& str, const state5d& state)
		{
			str << (nlohmann::json)state;
		}
	};
}