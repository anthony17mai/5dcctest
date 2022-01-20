#pragma once

#include <string>

namespace game
{
	namespace fen_library
	{
#define LIB_ENTRY(name) const char* name
		LIB_ENTRY(std_fen)		= "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
		LIB_ENTRY(queen_on_d5)	= "k7/8/8/4Q3/8/8/8/8";
		LIB_ENTRY(empty)		= "8/8/8/8/8/8/8/8";
		LIB_ENTRY(fools_mate)	= "rnb1kbnr/pppp1ppp/4*p3/8/6*Pq/5*P2/PPPPP2P/RNBQKBNR";
#undef LIB_ENTRY
	}
}