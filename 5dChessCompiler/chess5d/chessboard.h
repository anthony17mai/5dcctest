#pragma once

#include "game_parse.h"
#include <json.hpp>
#include "str_split.h"
#include "piececode.h"
#include "coord5d.h"
#include "chessmove.h"

namespace game
{
	class chessboard
	{
	public:
		using PARSE_MODE_SINGLE = standard_mode_tag;
		struct PARSE_MODE_JSON {};
		struct WRITE_MODE_RAW {};

		//something is wrong here
		struct row_it
		{
			using value_type = char*;
			using difference_type = ptrdiff_t;
			using reference = value_type&;
			using pointer = value_type*;
			using iterator_category = std::input_iterator_tag;	//cant write to this iterator e.g. cant write a "char*"

			const size_t boardh;	//I could use a statically 
			piececode* ptr;

			row_it operator++(int)
			{
				row_it tmp = *this;
				ptr += boardh;
				return tmp;
			}
			row_it& operator++()
			{
				ptr += boardh;
				return *this;
			}
			row_it operator--(int) 
			{
				row_it tmp = *this;
				ptr -= boardh;
				return tmp;
			}
			row_it& operator--() 
			{
				ptr -= boardh;
				return *this;
			}
			piececode* operator*() const { return ptr; }
			ptrdiff_t operator-(row_it r) { return this->ptr - r.ptr; }

			bool operator==(row_it o) { return ptr == o.ptr && boardh == o.boardh; }
			bool operator!=(row_it o) { return !((*this) == o); }

			piececode* begin()	{ return ptr; }
			piececode* end()	{ return ptr + boardh; }

			row_it(piececode* ptr, size_t boardh) : ptr(ptr), boardh(boardh) {}
			row_it(const row_it& o) : row_it(o.ptr, o.boardh) {}
		};
		typedef char* col_it;

	private:
		piececode* _arr;
	public:
		size_t boardw;
		size_t boardh;

		//get set
		size_t arr_size() const { return boardw * boardh; }
		size_t get_boardw() const { return boardw; }
		size_t get_boardh() const { return boardh; }

		/*
		Indexed using (x, y) with 0 <= x < boardw and 0 <= y < boardh
		*/
		piececode* operator[](size_t col_num) { return _arr + boardh * col_num; }
		const piececode* operator[](size_t col_num) const { return _arr + boardh * col_num; }
		piececode at(coord c) const { return (*this)[c.c][c.r]; }
		piececode& at(coord c) { return (*this)[c.r][c.c]; }

		void mutate(chessmove cm)
		{
			this->at(cm.to) = this->at(cm.from);
		}

		piececode* begin() { return _arr; }
		piececode* end() { return _arr + arr_size(); }
		row_it row_begin() { return row_it(_arr, boardh); }
		row_it row_end() { return row_it(end(), boardh); }
		piececode* col_begin(size_t col_num) { return _arr + boardh * (col_num); }
		piececode* col_end(size_t col_num) { return _arr + boardh * (col_num + 1); }
		
		//conversions
		/*
		converts the board into string
		*/
		explicit operator std::string()
		{
			std::string str = "";

			auto writeRow = [&](row_it iter)
			{
				int row_empties = 0;
				for (piececode c : iter)
				{
					if (piececode::CODE::NONE == c.code())
					{
						row_empties++; //count the number of empties
					}
					else
					{
						if (row_empties != 0)
						{
							//print number of empties
							str += std::to_string(row_empties);
						}

						//print the chesspiece
						str += write<piececode, piececode::PARSE_MODE_SINGLE>(c);

						//reset the number of empties
						row_empties = 0;
					}
				}

				if (row_empties != 0)
				{
					//print number of empties at the end of a row
					str += std::to_string(row_empties);
				}
			};

			//start at the top iterator and go down - top row first
			//avoid iter--
			auto iter = row_end();
			auto end = row_begin();
			while (--iter != end)
			{
				//write each row except the last
				writeRow(iter);

				//put a / between each
				str += '/';
			}

			//write last row
			writeRow(iter);

			return str;
		}
		/*
		exports the chessboard to fen
		*/
		std::string export_str() { return (std::string)(*this); }
		static std::pair<size_t, size_t> parse_board_size(const std::string& str)
		{
			std::vector<std::string> strings;
			AM_common::str_split(str, strings, '/');

			//find the board dimensions
			std::pair<size_t, size_t> dims;
			dims.first = 0;
			for (size_t i = 0; i < strings[0].size(); i++)
			{
				if (isdigit(strings[0][i]))
				{
					//increment by the number of whitespaces
					dims.first += strings[0][i] - '0';
				}
				else if (isalpha(strings[0][i]))
				{
					//increment by 1
					dims.first++;
				}
				else
				{
					//somthing here idk
				}
			}
			dims.second = strings.size();
			return dims;
		}
		static std::pair<size_t, size_t> parse_board_size(std::istream& str)
		{
			//find the board dimensions
			std::pair<size_t, size_t> dims(0, 1);
			std::string first;
			std::getline(str, first, '/');
			for (size_t i = 0; i < first.size(); i++)
			{
				if (isdigit(first[i]))
				{
					//increment by the number of whitespaces
					dims.first += first[i] - '0';
				}
				else if (isalpha(first[i]))
				{
					//increment by 1
					dims.first++;
				}
				else
				{
					//somthing here idk
				}
			}
			
			while (str.good())
			{
				std::string buf;
				std::getline(str, buf, '/');
				//keep getting line until end
				dims.second++;
			}
			return dims;
		}
		/*
		parses the chessboard from fen
		*/
		//obsolete
		/*
		static chessboard parse_dynamic_size(const std::string& str)
		{
			//find the board dimensions
			std::pair<size_t, size_t> dims;
			dims = parse_board_size(str);

			//parsing line by line
			chessboard cb(dims.first, dims.second);
			size_t x;
			int y;
			size_t i = 0;
			for (y = dims.second - 1; y > 0; y--)
			{
				auto pref_beg = str.begin() + i;
				decltype(pref_beg) pref_end;

				x = 0;
				while (true)
				{
					//if out of characters in the string then pretend like its fine
					if (!(i < str.size())) return cb;
					if (str[i] == '/')
					{
						//end line
						i++;
						break;
					}
					if (!(x < dims.first)) throw std::invalid_argument("chessboard dimensions missmatch");

					if (isalpha(str[i]))
					{
						//make a piececode
						//prefix does not include the piece code
						pref_end = str.begin() + i;
						std::string pref(pref_beg, pref_end);
						piececode pc(pref, str[i]);

						//add the piececode to the chessboard
						cb[y][x] = pc;

						i++;
						x++;
						pref_beg = str.begin() + i;
						continue;
					}

					if (isdigit(str[i]))
					{
						//write blanks explicitly since the piececode in memory is undefined
						int num = str[i] - '0';
						for (; num > 0; num--)
						{
							cb[y][x] = piececode();
							x++;
						}

						i++;
						pref_beg = str.begin();
						continue;
					}

					//if the code reaches this point then the character is a symbol
					i++;
				}
			}

			return cb;
		}
		*/
		chessboard& operator=(const chessboard& o)
		{
			delete[] _arr;

			boardw = o.boardw;
			boardh = o.boardh;

			_arr = new piececode[arr_size()];

			std::copy(o._arr, o._arr + arr_size(), _arr);
			return *this;
		}

		chessboard() : boardw(0), boardh(0) { _arr = nullptr; }
		chessboard(size_t boardw, size_t boardh) : boardw(boardw), boardh(boardh) 
		{
			_arr = new piececode[boardw * boardh];
		}
		chessboard(const chessboard& cb) : chessboard(cb.boardw, cb.boardh)
		{
			std::copy(cb._arr, cb._arr + arr_size(), _arr);
		}
		chessboard(chessboard&& cb) noexcept : chessboard(cb.boardw, cb.boardh)
		{
			//mv ctor
			this->_arr = cb._arr;
			cb._arr = nullptr;
		}

		~chessboard()
		{
			if (_arr != nullptr) delete[] _arr;
		}

		friend struct parser<chessboard>;
		friend struct writer<chessboard, chessboard::WRITE_MODE_RAW>;
	};

	template<> struct parser<chessboard, chessboard::PARSE_MODE_SINGLE>
	{
		parse_container<chessboard, chessboard::PARSE_MODE_SINGLE> operator()(std::istream& str)
		{
			//find the board dimensions
			std::pair<size_t, size_t> dims;
			std::streampos curr = str.tellg();
			dims = chessboard::parse_board_size(str);
			str.seekg(curr);

			//parsing line by line
			chessboard cb(dims.first, dims.second);
			size_t x;
			int y;
			for (y = dims.second - 1; y >= 0; y--)
			{
				x = 0;
				while (true)
				{
					//stop when the entire chessboard is full
					if (y == 0 && x == dims.first) return cb;

					char strchar = str.get();

					//if out of characters in the string then 
					if (!str.good()) return false;
					if (strchar == '/')
					{
						//end line
						break;
					}
					if (!(x < dims.first)) return false;

					if (isalpha(strchar))
					{
						//make a piececode
						//prefix does not include the piece code
						//std::string pref(pref_v.begin(), pref_v.end());
						str.unget();
						piececode pc = parser<piececode, piececode::PARSE_MODE_SINGLE>()(str);

						//add the piececode to the chessboard
						cb[y][x] = pc;

						x++;
						continue;
					}

					if (isdigit(strchar))
					{
						//write blanks explicitly since the piececode in memory is undefined
						//nevermind - the default constructor does this for you
						int num = strchar - '0';
						x += num;
						continue;
					}

					//if the code runs here then it encountered an unaccounted symbol
					return false;
				}
			}

			//code is not expected to reach this point
			return cb;
		}
	};

	template<> struct parse_container<chessboard, chessboard::PARSE_MODE_JSON>
	{
		chessboard cb;
		parse_container<board_id_t, board_id_t::PARSE_MODE_JSON> board_id;
		bool white = true;
		bool parse_success = false;
	};
	template<> struct parser<chessboard, chessboard::PARSE_MODE_JSON>
	{
		using container_t = parse_container<chessboard, chessboard::PARSE_MODE_JSON>;
		container_t operator()(std::istream& str)
		{
			container_t out;
			out.cb = parser<chessboard, chessboard::PARSE_MODE_SINGLE>()(str);
			if (str.get() != ':') return out;
			out.board_id = parser<board_id_t, board_id_t::PARSE_MODE_JSON>()(str);
			if (!out.board_id.parse_success) return out;
			if (str.get() != ':') return out;
			
			char col = str.get();
			if (col == 'w') out.white = true;
			else if (col == 'b') out.white = false;
			else return out;

			if (str.fail()) return out;

			out.parse_success = true;
			return out;
		}
	};

	template<> struct writer<chessboard, chessboard::WRITE_MODE_RAW> 
	{
		void operator()(std::ostream& str, const chessboard& cb)
		{
			for (size_t i = 0; i < cb.arr_size(); i++)
			{
				write(str, cb._arr[i]);
			}
		}
	};
}