#pragma once

#include <string>
#include <flag.h>
#include "game_parse.h"

namespace game
{
	struct piececode
	{
	public:
		static void set_move_flag(piececode& pc) { pc._attr &= ~(f_attr_t)ATTRIBUTES::UNMOVED; }

		//parses multi character 
		using PARSE_MODE_SINGLE = standard_mode_tag;
		struct PARSE_MODE_PGN {};

		enum class CODE : char
		{
			NONE = ' ',
			PAWN = 'p',
			KNIGHT = 'n',
			BISHOP = 'b',
			ROOK = 'r',
			QUEEN = 'q',
			KING = 'k',
			UNICORN = 'u',	//idk the codes
			DRAGON = 'd',
			PRINCESS = 's',
			RQUEEN = 'y',
			CKING = 'c',
			BRAWN = 'w'
		};
		enum class ATTRIBUTES : char
		{
			NONE = 0,
			WHITE = 1, BLACK = 0,
			UNMOVED = 2, NOT_UNMOVED = 0,

			//etc
		};
		using f_attr_t = AM_common::flag<ATTRIBUTES>;
		using f_code_t = AM_common::flag<CODE>;

		//fields
		f_code_t _code;
		f_attr_t _attr;

		//access
		f_code_t code() const { return _code; }
		f_attr_t attr() const { return _attr; }

		//parse

		//comparison
		bool is_empty() { return _code == CODE::NONE; }

		//conversion
		static char convert(f_code_t c, f_attr_t attr)
		{
			if (attr & ATTRIBUTES::WHITE)
			{
				//case white
				return toupper((char)c);
			}
			else
			{
				//case black
				return (char)c;
			}
		}
		static inline ATTRIBUTES make_attr(std::string suff, char c)
		{
			f_attr_t attr = ATTRIBUTES::NONE;
			if (isupper(c))
			{
				attr |= ATTRIBUTES::WHITE;
			}
			else if (islower(c))
			{
				//(unnecessary but explicit)
				attr |= ATTRIBUTES::BLACK;
			}

			//only valid suffix is *?

			if (suff == "*")
			{
				//unmoved
				attr |= ATTRIBUTES::UNMOVED;
			}
			else
			{
				//attr |= ATTRIBUTES::MOVED;
			}
			return attr;
		}
		//USE SUFFIX NOT PREFIX
		static CODE make_code(std::string suff, char c)
		{
			//tecnically the c == none case is unnecessary since tolower() already accounts for non upper 
			if ((CODE)c == CODE::NONE)
			{
				return CODE::NONE;
			}
			else
			{
				return (CODE)tolower(c);
			}
		}
		/*
		this doesnt allow me to add a prefix such as * to indicate a moved / unmoved pawn
		*/
		explicit operator char() const
		{
			char _c =  convert(_code, _attr);
			return _c;
		}
		piececode(char c) : piececode(std::string(), c) {}
		/*
		turn into cast operator
		*/
		std::string write()
		{
			std::string str;
			char label = (char)(*this);

			//add suffix
			str += label;
			
			if (_attr & ATTRIBUTES::UNMOVED)
			{
				str += '*';
			}

			return str;
		}

		//e
		void move_flag() { _attr &= ~AM_common::make_flag(ATTRIBUTES::UNMOVED); }

		//ctor
	private:
		piececode(CODE c, ATTRIBUTES a) : _code(c), _attr(a) {}
	public:
		//SUFFIX NOT PREFIX
		piececode(std::string suff, char code) : _code(make_code(suff, code)), _attr(make_attr(suff, code)) {}
		piececode() : _code(CODE::NONE), _attr(ATTRIBUTES::NONE) {}

		friend struct parser<piececode, PARSE_MODE_SINGLE>;
		friend struct parser<piececode, PARSE_MODE_PGN>;
		friend struct parser<piececode, PARSE_MODE_SINGLE>;
	};

	/*
	parses a one or two char piececode from the stream
	the parser is not able to parse attributes
	*/
	template<> struct parser<piececode, piececode::PARSE_MODE_PGN>
	{
		parse_container<piececode> operator()(std::istream& str) 
		{
			//read the piececode
			char c = str.get();

			//if c is not caps then this parse fails
			if (!isupper(c)) return false;

			using AM_common::flag;
			flag<piececode::CODE> code = (flag<piececode::CODE>)tolower(c);

			if (code == 'r')
			{
				//rq
				if (tolower(str.peek()) == 'q')
				{
					code = piececode::CODE::RQUEEN;
					str.get();
				}
				else
				{
					//just rook
				}
			}
			else if (code == 'p')
			{
				//pr
				if (str.peek() ==  'r')
				{
					code = piececode::CODE::PRINCESS;
					str.get();
				}
				else
				{
					//just pawn
				}
			}

			//cannot parse attributes
			flag<piececode::ATTRIBUTES> attr = (flag<piececode::ATTRIBUTES>)0;

			if (str.fail())
				return false;
			else
				return piececode(code, attr);
		}
	};

	template<> struct parser<piececode, piececode::PARSE_MODE_SINGLE>
	{
		parse_container<piececode> operator()(std::istream& str)
		{
			//parses a single character for a piececode then parses a suffix. only valid suffix is asterisk
			char c = str.get();

			std::string suff;
			if (str.peek() == '*')
			{
				suff = '*';
				str.get();
			}
			else
			{
				suff = "";
			}

			return piececode(suff, c);
		}
	};

	template<> struct writer<piececode, piececode::PARSE_MODE_SINGLE>
	{
		void operator()(std::ostream& out, const piececode& pc)
		{
			char label = (char)(pc);

			//add label
			out << label;

			if (pc.attr() & piececode::ATTRIBUTES::UNMOVED)
			{
				out << "*";
			}
		}
	};
}