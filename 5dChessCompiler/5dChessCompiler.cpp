// 5dChessCompiler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <json.hpp>
#include <str_split.h>
#include <casefreecmp.h>
#include <fstream>
#include <filesystem>
#include "state5d.h"
#include "state5dparser.h"
#include "symbol_lookup_table.h"

using namespace nlohmann;

//5dcc [-s, -h, -5dpgn_version=number] <-root_chessboard-> <5dpgn> <out_file>
int main(int argc, char* argv[])
{
	/*
	enum
	{
		//ROOT_CHESSBOARD,
		PGN,
		OUT_FILE,
		NUM_ARGS
	};

	bool soft_compile = false;
	int argument_num = 0;
	std::string files[NUM_ARGS];

	//command line parsing
	for (int i = 1; argument_num < NUM_ARGS; i++)
	{
		std::string str = argv[i];

		if (str[0] == '-')
		{
			//option
			if (AM_common::casefreeeqq()(str, "-s"))
			{
				//soft compile
				soft_compile = true;
			}
			else if (AM_common::casefreeeqq()(str, "-h"))
			{
				//hard compile
				soft_compile = false;
			}
		}
		else
		{
			//parameter
			files[argument_num] = str;

			argument_num++;
		}

		if (i < argc)
			continue;
		else
		{
			std::cout << "usage: 5dcc [-s, -h, -5dpgn_version=number] <-root_chessboard-> <5dpgn> <out_file>" << std::endl;
			return 1;
		}
	}

	//open the 5dpgn	
	std::ifstream pgn(files[PGN]);
	*/
	using namespace game;

	std::filesystem::path wd = std::filesystem::current_path();
	std::filesystem::path fil = std::filesystem::path(wd).append("test.txt");
	std::filesystem::path lookup_table = wd.parent_path().append("lookup.txt");

	//build the lookup table
	game::populate_lookup_table(std::ifstream(lookup_table));

	std::ifstream test_file(fil);
	parse_container<state5d, state5d::PARSE_MODE_PGN5D> state = parse<state5d, state5d::PARSE_MODE_PGN5D>(test_file);

	if (!state.parse_success) std::cout << "parse failiure" << std::endl;
	else writer<state5d, state5d::PARSE_MODE_JSON>()(std::cout, state.ob);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
