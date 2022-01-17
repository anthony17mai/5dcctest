#pragma once

#include "chessmove_parser.h"

namespace game
{
	struct many_chessmove
	{
		std::vector<chessmove> moves_white;
		std::vector<chessmove> moves_black;
	};

	template<> struct parse_container<many_chessmove>
	{
		bool parse_success = false;
		int turnnum = 0;

		std::vector<parse_container<chessmove>> moves_white;
		std::vector<parse_container<chessmove>> moves_black;

		//container is a holder for the parts and pieces which need a state in order to build a chessmove.
		many_chessmove construct(const state5d& state)
		{
			many_chessmove res;
			for (auto& container : moves_white)
			{
				res.moves_white.push_back(container.construct(state, true));
			}
			for (auto& container : moves_black)
			{
				res.moves_black.push_back(container.construct(state, false));
			}
			return res;
		}
	};

	//n. [chessmove]... /  [chessmove]...
	//n. [movelist] / [movelist]
	//n. [chessmove]... \n
	template<> struct parser<many_chessmove>
	{
		parse_container<many_chessmove> operator()(std::istream& str)
		{
			parse_container<many_chessmove> container;

			std::string move_number;
			std::getline(str, move_number, '.');
			int move_num = std::stoi(move_number);

			while (isspace(str.peek())) str.get();
			
			while (str.good() && str.peek() != '/')
			{
				//parse chessmoves until out of chessmoves
				parse_container<chessmove> cm = parser<chessmove>()(str);
				container.moves_white.push_back(cm);

				if (str.fail()) return container;

				//make sure to skip ws just in case
				while (isspace(str.peek())) str.get();
			}

			if (str.peek() == '/')
			{
				// '/'
				str.get();
			}
			else
			{
				//if no '/' is found then only white moves. exit success
				container.parse_success = true;
				return container;
			}

			while (isspace(str.peek())) str.get();

			//read chessmoves until end of stream
			while (str.good())
			{
				//parse chessmoves until out of chessmoves
				parse_container<chessmove> cm = parser<chessmove>()(str);
				container.moves_black.push_back(cm);

				//make sure to skip ws
				while (isspace(str.peek())) str.get();
			}

			container.parse_success = true;
			return container;
		}
	};
}