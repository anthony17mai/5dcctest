#pragma once

namespace game
{
	//should have just used std::pair
	template<typename integ>
	struct pair
	{
		//obsolete
		static pair parse(const char* c_str)
		{
			//assume perfect input
			char x = tolower(c_str[0]);	//letter
			char y = tolower(c_str[1]);	//number

			return pair(x - 'a', y - '1');
		}

		integ c;
		integ r;

		bool operator==(const pair& o) const { return c == o.c && r == o.r; }

		pair(char c, char r) : c(c), r(r) {}
		pair() : c(0), r(0) {}
	};

	typedef pair<uint8_t> coord;

	//parser for coord
	template<> struct parser<coord>
	{
		parse_container<coord> operator()(std::istream& str)
		{
			char cs[2];
			str.read(cs, 2);

			char x = cs[0];
			char y = cs[1];

			if (!str.fail())
				return coord(x - 'a', y - '1');
			else
				return false;
		}
	};

	//todo: make struct
	typedef int L_idx_t;
	/*
	converts from a Time number to an index
	*/
	struct T_idx_t
	{
	public:
		static inline T_idx_t construct_raw(int raw) { return T_idx_t(raw, 0); }

	private:
		int _val;

	public:
		inline constexpr int get() const { return _val; }
		//gets value
		inline constexpr operator int()
		{
			return get();
		}

		/*
		cast operator which converts the timeline using timeline number
		*/
		inline T_idx_t(int timeline_number) : _val(timeline_number - 1) { }
		T_idx_t(const T_idx_t&) = default;

	private:
		inline T_idx_t(int raw, int) : _val(raw) { }
	};

	struct board_id_t : public std::pair<int, int>
	{
		using PARSE_MODE_PGN = standard_mode_tag;
		struct PARSE_MODE_JSON {};

		using std::pair<int, int>::pair;
	};

	struct timeline_number
	{
		using TIMELINE_LABEL_FORMAT = standard_mode_tag;

		//value = bits - even && sign
		//bits = value + even && sign
		static timeline_number construct(bool even, int tl_num) { return timeline_number(tl_num - (!even && (tl_num < 0))); }
		static timeline_number neg_zero() { return timeline_number(std::numeric_limits<integ>::max()); }

		typedef unsigned int integ;
		integ _bits;
		enum
		{
			sign_bit = ~(std::numeric_limits<integ>::max() >> 1),
		};

		integ sign() const { return _bits & sign_bit; }
		integ magnitude() const { return sign() ? ~_bits : _bits; }
		integ value(bool even) const { return _bits + (!even && sign()); }

		timeline_number() : _bits(0) {}

	private:
		timeline_number(integ val) : _bits(val) {}

		friend struct parser<timeline_number>;
	};
	template<> struct parser<timeline_number>
	{
		parse_container<timeline_number> operator()(std::istream& str)
		{
			if (str.peek() == '-')
			{
				str.get();
				std::string string;
				std::getline(str, string, ' ');
				unsigned int integ = std::stoul(string);
				return timeline_number(~integ);	//ones complement negation
			}
			else if (str.peek() == '+')
			{
				str.get();
				std::string string;
				std::getline(str, string, ' ');
				unsigned int integ = std::stoul(string);
				return timeline_number(integ);
			}
			else
			{
				std::string string;
				std::getline(str, string, ' ');
				unsigned int integ = std::stoul(string);
				return timeline_number(integ);
			}
		}
	};
	template<> struct writer<timeline_number, timeline_number::TIMELINE_LABEL_FORMAT>
	{
		void operator()(std::ostream& str, timeline_number num)
		{
			//timeline_number represents a 1s complement number used to label the timeline
			if (num.sign()) str << "-";
			else str << "+";
			str << abs((int)num._bits + (num.sign() == timeline_number::sign_bit));
			str << "L";
		}
	};

	template<> struct parse_container<board_id_t, board_id_t::PARSE_MODE_PGN>
	{
		bool parse_success = false;
		timeline_number tl_num;
		int T_val = 0;

		board_id_t construct(bool isEven)
		{
			//if isEven then shift all the negative boardids down by one
			return board_id_t(T_val, tl_num.value(isEven));
		}
	};
	template<> struct parse_container<board_id_t, board_id_t::PARSE_MODE_JSON> : public parse_container<board_id_t, board_id_t::PARSE_MODE_PGN> { 
		using parse_container<board_id_t, board_id_t::PARSE_MODE_PGN>::parse_container;
	};
	/*
	([L]<a> T<b>)
	*/
	template<> struct parser<board_id_t, board_id_t::PARSE_MODE_PGN>
	{
		parse_container<board_id_t> operator()(std::istream& str)
		{
			parse_container<board_id_t> out;

			//assert that first char is (
			if (str.get() != '(')
			{
				return out;
			}

			//skip if L is found - case free
			if (toupper(str.peek()) == 'L')
			{
				str.get();
			}

			//read <a>
			std::string first;
			std::getline(str, first, 'T');

			//read <b>
			std::string sec;
			std::getline(str, sec, ')');

			int time = std::stoi(sec);

			out.tl_num = parse<timeline_number>(first);
			out.T_val = time;

			if (str.fail()) return out;

			out.parse_success = true;

			return out;
		}
	};
	template<> struct parser<board_id_t, board_id_t::PARSE_MODE_JSON>
	{
		using container_t = parse_container<board_id_t, board_id_t::PARSE_MODE_JSON>;
		container_t operator()(std::istream& str)
		{
			container_t out;

			std::string L_number, T_number;

			std::getline(str, L_number, ':');
			std::getline(str, T_number, ':');
			str.unget();	//parse only the numbers (this can be removed if i wasnt so lazy

			if (str.fail()) return out;

			out.tl_num = parse<timeline_number>(L_number);
			out.T_val = std::stoi(T_number);
			out.parse_success = true;

			return out;
		}
	};

	//5d coordnate used to uniquely identify a 5d coordnate
	struct coord5d
	{
		coord _coord;
		board_id_t _board_id;
		bool is_white_board = 0;

		operator coord()
		{
			return _coord;
		}

		coord5d() = default;
		coord5d(coord c, board_id_t id, bool is_white_board) : _coord(c), _board_id(id), is_white_board(is_white_board) {}
	};
}