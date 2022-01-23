#include "CppUnitTest.h"

#include "many_chessmove.h"
#include "chessmove_parser.h"
#include "state5d.h"
#include "fen_library.h"
#include "chessmove_parser.h"
#include "many_chessmove.h"
#include "state5dparser.h"
#include <fstream>
#include <filesystem>

#include "symbol_lookup_table.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace Microsoft { namespace VisualStudio { namespace CppUnitTestFramework {
	template<> inline std::wstring ToString<nlohmann::json>(const nlohmann::json& t) { return ToString(t.dump()); }
} } }

namespace unittests
{
	using namespace game;
	using nlohmann::json;

	const std::filesystem::path w_dir = std::filesystem::current_path().parent_path().append("5dcc_test");
	const std::filesystem::path lookup_table = w_dir.parent_path().append("lookup.txt");

	TEST_CLASS(state5dtest)
	{
	public:		
		TEST_METHOD(json_import_export)
		{
			//import a json
			//this should be the expected output so it has to include a + here
			json test_state = json::parse(R"({"+0L":["rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"]})");
			
			Assert::IsTrue(test_state["+0L"].is_array());
			//to array
			auto iter = test_state["+0L"].begin();
			Assert::IsTrue(iter[0].is_string());

			state5d state = state5d(test_state);

			//check the 0l timeline len
			Assert::IsTrue(state.get_timeline(0)->size() > 0);

			//check the offset (implicit -1)
			Assert::AreEqual(-1, state.get_timeline(0)->_offset());

			//try exporting
			json res = (json)state;

			std::string res_str = res.dump();
			std::string test_str = test_state.dump();

			Assert::AreEqual(test_str, res_str);
		}

		TEST_METHOD(pgn_parser)
		{
			auto check = [&](std::string e_file, std::string pgn_file) 
			{
				std::filesystem::path fil = std::filesystem::path(w_dir).append(pgn_file);
				std::filesystem::path e_out = std::filesystem::path(w_dir).append(e_file);
				std::fstream test_rw(fil, std::ios_base::in);
				std::fstream e_rw(e_out, std::ios_base::in);
				auto state = parse<state5d, state5d::PARSE_MODE_PGN5D>(test_rw);

				//make sure parse is success
				Assert::IsTrue(state);

				nlohmann::json expected_json;
				e_rw >> expected_json;
				state5d expected(expected_json);

				//make sure json objects are identical
				Assert::AreEqual((nlohmann::json)expected, (nlohmann::json)state.ob);
			};

			//royalty war
			check("e_rw.txt", "test_rw.5dpgn");

			//royalty war with some moves
			check("e_rw_moves.txt", "test_rw_moves.5dpgn");
		}
	};

	TEST_CLASS(test_chessboard)
	{
	public:
		TEST_METHOD(json_import_export)
		{
			//only imports
			chessboard blank(8, 8);
			Assert::IsTrue(blank.get_boardw() == 8);
			Assert::IsTrue(blank.get_boardh() == 8);
			Assert::AreEqual((std::string)blank, std::string("8/8/8/8/8/8/8/8"));
			blank[7][0] = 'p';
			Assert::AreEqual((std::string)blank, std::string("p7/8/8/8/8/8/8/8"));
			std::fill(blank[3], blank[4], 'k');
			Assert::AreEqual((std::string)blank, std::string("p7/8/8/8/kkkkkkkk/8/8/8"));
			blank[6][4] = 'n';
			Assert::AreEqual((std::string)blank, std::string("p7/4n3/8/8/kkkkkkkk/8/8/8"));


			//import plus export
			chessboard cb = parse<chessboard>("8/8/8/8/8/8/8/8");
			std::string export_str = (std::string)cb;
			Assert::AreEqual(std::string("8/8/8/8/8/8/8/8"), export_str);
			cb = parse<chessboard>("p7/8/8/8/8/8/8/8");
			export_str = (std::string)cb;
			Assert::AreEqual(std::string("p7/8/8/8/8/8/8/8"), export_str);
			cb = parse<chessboard>(fen_library::std_fen);
			export_str = (std::string)cb;
			Assert::AreEqual(std::string(fen_library::std_fen), export_str);

			//try with an asterisk
			auto cb_container = parse<chessboard>("p*7/8/8/8/8/8/8/8");
			Assert::IsTrue(cb_container.parse_success);
			export_str = (std::string)(chessboard)cb_container;
			Assert::AreEqual(std::string("p*7/8/8/8/8/8/8/8"), export_str);
		}
	};

	TEST_CLASS(other_parsers_i_guess)
	{
	public:
		TEST_METHOD(coordnate_parser)
		{
			coord c = parse<coord>("a1");
			Assert::IsTrue(c == coord(0, 0));
			c = parse<coord>("b2");
			Assert::IsTrue(c == coord(1, 1));
			c = parse<coord>("c7");
			Assert::IsTrue(c == coord(2, 6));
		}

		TEST_METHOD(piececode_parser)
		{
			auto container = parse<piececode, piececode::PARSE_MODE_SINGLE>("r");
			Assert::IsTrue(container);
			piececode pc = container;
			Assert::IsTrue(pc.code() == piececode::CODE::ROOK);
			Assert::IsTrue(pc.attr() == piececode::ATTRIBUTES::BLACK);

			pc = parse<piececode, piececode::PARSE_MODE_PGN>("B");
			Assert::IsTrue(pc.code() == piececode::CODE::BISHOP);
			Assert::IsTrue(pc.attr() == piececode::ATTRIBUTES::BLACK);	//parser should default to black since PGN does not signify color or attributes

			pc = parse<piececode, piececode::PARSE_MODE_PGN>("RQ");
			Assert::IsTrue(pc.code() == piececode::CODE::RQUEEN);
			Assert::IsTrue(pc.attr() == piececode::ATTRIBUTES::BLACK);

			pc = parse<piececode, piececode::PARSE_MODE_SINGLE>("R");
			Assert::IsTrue(pc.code() == piececode::CODE::ROOK);
			Assert::IsTrue(pc.attr() == (AM_common::make_flag(piececode::ATTRIBUTES::WHITE) | piececode::ATTRIBUTES::NOT_UNMOVED));

			pc = parse<piececode, piececode::PARSE_MODE_SINGLE>("p*");
			Assert::IsTrue(pc.code() == piececode::CODE::PAWN);
			Assert::IsTrue(pc.attr() == (AM_common::make_flag(piececode::ATTRIBUTES::BLACK) | piececode::ATTRIBUTES::UNMOVED));	//unmoved pawn
		}

		TEST_METHOD(chessmove_parser)
		{
			game::populate_lookup_table(std::ifstream(lookup_table));

			state5d standard = parse<state5d>(R"({"0L":["rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"]})");
			auto container = parse<chessmove>("(0T1)Ng1f3");

			chessmove cm = container.construct(standard, true);

			Assert::IsTrue(container.cp.code() == 'n');
			Assert::IsTrue(cm.from_board == board_id_t(1, 0));
			Assert::IsTrue(cm.to_board == board_id_t(1, 0));
			Assert::IsTrue(cm.from == coord(6, 0));
			Assert::IsTrue(cm.to == coord(5, 2));

			//try an implicit move

			container = parse<chessmove>("Ng1f3");
			cm = container.construct(standard, true);

			Assert::IsTrue(container.cp.code() == 'n');
			Assert::IsTrue(cm.from_board == board_id_t(1, 0));
			Assert::IsTrue(cm.to_board == board_id_t(1, 0));
			Assert::IsTrue(cm.from == coord(6, 0));
			Assert::IsTrue(cm.to == coord(5, 2));

			//time jump

			container = parse<chessmove>("(0T7)Ng4>>(0T6)g2");
			state5d sample_chessboard = parser<state5d, state5d::PARSE_MODE_PGN5D>()(std::ifstream(std::filesystem::path(w_dir).append("samplepgn1.5dpgn")));
			cm = container.construct(sample_chessboard, false);

			Assert::IsTrue(container.cp.code() == 'n');
			Assert::IsTrue(cm.from_board == board_id_t(7, 0));
			Assert::IsTrue(cm.to_board == board_id_t(6, 0));
			Assert::IsTrue(cm.from == coord(6, 3));
			Assert::IsTrue(cm.to == coord(6, 1));
		}

		TEST_METHOD(chessmove_list_parser)
		{
			state5d standard = parse<state5d>(R"({"0L":["rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"]})");

			//parse a chessmove list
			auto container = parse<many_chessmove>("1. Ng1f3 / Ng8f6");

			Assert::IsTrue(container.construct(standard).moves_white[0] == parse<chessmove>("Ng1f3").construct(standard, true));
			Assert::IsTrue(container.construct(standard).moves_black[0] == parse<chessmove>("Ng8f6").construct(standard, false));
		}
	};
}
