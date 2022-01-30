#pragma once

#include "chessboard.h"
#include "coord5d.h"
#include "bi_vector.h"
#include "casefreecmp.h"
#include <json.hpp>
#include <iostream>
#include <unordered_set>

namespace game
{
	//fw decl
	class state5d;

	extern std::unordered_map<std::string, state5d> lookup_table;

	//i dont think this is necessary
	template<> struct writer<chessboard*>
	{
		void operator()(std::ostream& str, const chessboard* cb)
		{
			if (cb == nullptr) str << "null";
			else str << (std::string)(chessboard)*cb;
		}
	};

	class state5d
	{
	public:
		using PARSE_MODE_JSON = standard_mode_tag;
		struct PARSE_MODE_PGN5D {};

		static state5d lookup(std::string variant_name)
		{
			//if the map contains the key
			if (lookup_table.count(variant_name) != 0)
			{
				//look up the position and then return a copy
				return lookup_table.at(variant_name);
			}
			else
			{
				//return an empty
				return state5d();
			}
		}

		/*
		chessboard pointer is a unique pointer so that I dont have to mess with
		*/
		using cb_ptr = std::unique_ptr<chessboard>;
		/*
		instead of storing both the blackand white boards in the same list, simply pair the two(makes a lot of calculations simpler - eg, you dont have to skip every other board)
		fields nullable
		inherits from std::pair so that I can provide a valid copy constructor
		*/
		struct color_pair : public std::pair<cb_ptr, cb_ptr> 
		{ 
			cb_ptr& at(bool col) { return col ? first: second; }

			using std::pair<cb_ptr, cb_ptr>::pair; 
			color_pair() : std::pair<cb_ptr, cb_ptr>(nullptr, nullptr) {}
			color_pair(const color_pair& o) : color_pair() 
			{
				//copy over if non null
				if (o.first.get() != nullptr)
				{
					first = cb_ptr(new chessboard(*o.first.get()));
				}

				if (o.second.get() != nullptr)
				{
					second = cb_ptr(new chessboard(*o.second.get()));
				}
			}
			color_pair(color_pair&& o) noexcept : std::pair<cb_ptr, cb_ptr>(std::move(o.first), std::move(o.second))
			{
				//move the unique pointers
			}
		};
		/*
		represents a single timeline
		*/
		using horizontal = AM_common::bi_vector<color_pair>;
		/*
		represents a vector of timelines
		*/
		using vertical = AM_common::bi_vector<horizontal>;
		/*
		iterator traversing the L dimension
		*/
		using L_iter = vertical::iterator;
		using const_L_iter = vertical::const_iterator;
		/*
		iterator traversing the t dimension
		*/
		using T_iter = horizontal::iterator;
		using const_T_iter = horizontal::const_iterator;
		/*
		double access iterator
		replaces the pointed type with L_iter
		*/
		using iterator = T_iter;
		using index_val = horizontal::off_t;

	private:
		static const color_pair NON_NULL_EMPTY_PAIR;

		/*
		the full board state
		could have paired horizontal and iter_T
		using L_entry = std::pair<horizontal, iter_T>;
		*/
		vertical _board_list;
		bool _is_even;

	public:
		static inline color_pair make_color_pair(chessboard* cb, bool is_white)
		{
			return is_white ? color_pair(cb, nullptr) : color_pair(nullptr, cb);
		}

		//get
		bool is_empty() const { return _board_list.size() == 0; }
		bool is_even() const { return _is_even; }
		int num_tl() const { return _board_list.size(); }
		timeline_number timeline_label(index_val L_index) const { return timeline_number::construct(_is_even, L_index); }

		//random access
		/*
		returns iterator to entry
		*/
		inline L_iter get_timeline(index_val T_index)
		{
			return _board_list.zero() + T_index;
		}
		inline L_iter get_timeline(const coord5d& coord)
		{
			return get_timeline(coord._board_id.first);
		}
		inline const_L_iter get_timeline(index_val T_index) const
		{
			return _board_list.zero() + T_index;
		}
		/*
		what to do with even timelines
		*/
		static inline T_iter get_board_pair(L_iter entry, index_val T_index) { return entry->at(T_index); }
		inline T_iter get_board_pair(index_val T_index, index_val L_index) { return get_board_pair(get_timeline(L_index), T_index); }
		inline T_iter get_board_pair(const coord5d& coord) { return get_board_pair(coord._board_id.first, coord._board_id.second); }
		/*
		gets chessboard at location
		*/
		static inline chessboard* get_board(T_iter pair, bool is_white)
		{
			//hopefully this compiles to be branch free?
			//if not then i know a way to make it branch free easily
			return is_white ?
				pair->first.get() :
				pair->second.get();
		}
		inline chessboard* get_board(const coord5d& coord) { return get_board(get_board_pair(coord), coord.is_white_board); }
		/*
		accesses the timeline skips the entry
		board access using several index operations - although the board pair cannot index
		*/
		inline horizontal& operator[](index_val T_index)
		{
			return _board_list[T_index];
		}
		inline const horizontal& operator[](index_val T_index) const
		{
			return _board_list[T_index];
		}
		/*
		accesses a piece
		*/
		inline piececode& operator[](const coord5d& coord)
		{
			auto brd = get_board(coord);
			return brd->at(coord._coord);
		}
		inline T_iter head_ptr(index_val L_index)
		{
			horizontal& tl = _board_list[L_index];
			return tl.head_ptr();
		}
		inline coord5d head_co(index_val L_index) const
		{
			coord5d c5d;
			const horizontal& tl = _board_list[L_index];
			const color_pair& pair = tl.head();
			index_val T_index = tl.head_idx();

			c5d._board_id = std::make_pair(T_index, L_index);

			//find the color of head
			if (pair.second == nullptr)
			{
				c5d.is_white_board = true;
			}
			else
			{
				c5d.is_white_board = false;
			}

			return c5d;
		}
		inline chessboard* head_cb(index_val L_index)
		{
			horizontal& tl = _board_list[L_index];
			color_pair& pair = tl.head();

			return pair.second == nullptr ? pair.first.get() : pair.second.get();
		}

		//bounds checking
		/*
		checks if the pair contains the chessboard color
		*/
		static inline bool contains(color_pair entry, bool white)
		{
			return white ? entry.first.get() != nullptr : entry.second.get() != nullptr;
		}
		/*
		checks if the index is in bound of an entry
		*/
		static inline bool contains(const_L_iter entry, index_val idx) { return entry->contains_idx(idx); }
		/*
		basically checks if board (x, y) is found inside of the vectors
		*/
		inline bool contains(index_val T_index, index_val L_idx) const { return contains_tl(T_index) && contains(get_timeline(T_index), L_idx); }
		/*
		T coordnate exists
		*/
		inline bool contains_tl(index_val T_index) const { return _board_list.contains_idx(T_index); }
		inline bool contains(coord5d coord) const 
		{
			if (contains_tl(coord._board_id.second))
			{
				auto tl = get_timeline(coord._board_id.second);
				return contains(tl, coord._board_id.first) && contains(*tl->at(coord._board_id.first), coord.is_white_board);
			}
			else return false;
		}
		static bool tail(const color_pair& p) { return p.second.get() == nullptr; }
		static index_val tail(const_L_iter tl) { return tl->tail_idx(); }

		//expansion operations
		/*
		creates a new timeline above the current timeline group
		*/
		template<typename... _init>
		inline chessboard* make_timeline_up(index_val T_index, bool is_white, _init... initializer)
		{
			chessboard* brd = new chessboard(initializer...);
			color_pair pair = is_white ? color_pair(std::unique_ptr<chessboard>(brd), std::unique_ptr<chessboard>((chessboard*)nullptr)) : color_pair(std::unique_ptr<chessboard>((chessboard*)nullptr), std::unique_ptr<chessboard>(brd));
			horizontal timeline;
			timeline.push_back(std::move(pair));
			timeline.shift_bw(T_index);
			_board_list.push_back(std::move(timeline));
			return brd;
		}
		/*
		creates a new timeline below the current timeline group
		*/
		template<typename... _init>
		inline chessboard* make_timeline_down(index_val T_index, bool is_white, _init... initializer)
		{
			chessboard* brd = new chessboard(initializer...);
			color_pair pair = is_white ? color_pair(std::unique_ptr<chessboard>(brd), std::unique_ptr<chessboard>((chessboard*)nullptr)) : color_pair(std::unique_ptr<chessboard>((chessboard*)nullptr), std::unique_ptr<chessboard>(brd));
			horizontal timeline;
			timeline.push_back(std::move(pair));
			timeline.shift_bw(T_index);
			_board_list.push_front(std::move(timeline));
			return brd;
		}
		/*
		make a new chessboard at the end of the timeline
		*/
		template<typename... _init>
		inline chessboard* push_timeline(index_val L_index, const _init&... init)
		{
			std::unique_ptr<chessboard> new_cb = std::make_unique<chessboard>(init...);
			horizontal& tl = _board_list[L_index];
			color_pair& pair = tl.tail();
			auto ret = new_cb.get();

			if (pair.second == nullptr)
			{
				pair.second = std::move(new_cb);
			}
			else
			{
				tl.push_back(std::move(std::make_pair(std::move(new_cb), cb_ptr(nullptr))));
			}
			//facepalm
			//return new_cb.get();
			return ret;
		}
		/*
		makes a chessmove
		*/
		bool move(const chessmove& cm)
		{
			//assert from and to exist
			if (!contains(cm.from_coord())) return false;
			if (!contains(cm.to_coord())) return false;

			//find piece code
			auto f_tl = get_timeline(cm.from_board.second);
			//assert that from is an active timeline
			if (cm.from_board.first != tail(f_tl)) return false;
			if (cm.color != tail(f_tl->tail())) return false;
			piececode pc = f_tl->at(cm.from_board.first)->at(cm.color)->at(cm.from);
			piececode::set_move_flag(pc);

			//if the to chessboard is at the tail of a timeline
			auto t_tl = get_timeline(cm.to_board.second);
			if (cm.to_board.first == tail(t_tl))
			{
				//non branching move
				if (cm.to_board == cm.from_board)
				{
					//non jumping move
					const chessboard* t_cb = get_board(cm.to_coord());
					
					//push a new chessboard to the timeline
					chessboard* new_t_cb = push_timeline(cm.to_board.second, *t_cb);

					//move the piece
					new_t_cb->at(cm.from) = piececode();
					new_t_cb->at(cm.to) = pc;
				}
				else
				{
					//jump
					const chessboard* f_cb = get_board(cm.from_coord());

					//push the travel from board
					chessboard* new_f_cb = push_timeline(cm.from_board.second, *f_cb);
					
					//remove the traveling piece
					new_f_cb->at(cm.from) = piececode();

					//put the traveling piece in the new chessboard
					const chessboard* t_cb = get_board(cm.to_coord());
					chessboard* new_t_cb = push_timeline(cm.to_board.second, *t_cb);
					new_t_cb->at(cm.to) = pc;
				}
			}
			else
			{
				//branching move
				bool ncb_col = !cm.color;
				index_val ncb_off = ncb_col ? cm.to_board.first + 1 : cm.to_board.first;	//shift the new chessboard over by a half turn
				chessboard *n_t_cb, *n_f_cb;
				const chessboard* f_cb = get_board(cm.from_coord());
				const chessboard* t_cb = get_board(cm.to_coord());

				n_f_cb = push_timeline(cm.from_board.second, *f_cb);
				if (cm.color)
				{
					n_t_cb = make_timeline_up(ncb_off, ncb_col, *t_cb);
				}
				else
				{
					n_t_cb = make_timeline_down(ncb_off, ncb_col, *t_cb);
				}

				//remove the traveling piece
				std::string _n_f, _n_t;
				_n_f = write(n_f_cb);
				_n_t = write(n_t_cb);

				n_f_cb->at(cm.from) = piececode();

				//place the traveling piece
				n_t_cb->at(cm.to) = pc;
			}
			return true;
		}

		//conversions
		//produces a "Timelines" object
		//TODO: restructure the json result
		inline explicit operator nlohmann::json() const
		{
			nlohmann::json timelines;

			const_L_iter zero_l = _board_list.at(0);

			//take each timeline
			for (const_L_iter timeline = _board_list.begin(); timeline < _board_list.end(); timeline++)
			{
				//the timeline array
				nlohmann::json tl_arr = nlohmann::json::array();

				//count the number of nulls to write
				int num_blanks = timeline->begin_idx() - zero_l->begin_idx();
				while (num_blanks > 0)
				{
					tl_arr.push_back((nlohmann::json)nullptr);
					tl_arr.push_back((nlohmann::json)nullptr);
					num_blanks--;
				}
				
				//find L - who cares about speed anyways
				index_val L_num = timeline - _board_list.zero();
				std::string tag = write(timeline_label(L_num));

				//take each board and add it to the list
				for (size_t i = 0; i < timeline->size(); i++)
				{
					const_T_iter board_pair = timeline->begin() + i;

					//check that the array contains the right number of elements
					/*
					if (tl_arr.size() != 2 * i)
						throw -1;
					*/

					//convert each board to a string
					if (board_pair->first.get() == nullptr)
					{
						//im just gonna trust that this works
						tl_arr.push_back((nlohmann::json)nullptr);
					}
					else
					{
						tl_arr.push_back((std::string)(*board_pair->first));
					}

					if (board_pair->second == nullptr)
					{
						tl_arr.push_back((nlohmann::json)nullptr);
					}
					else
					{
						tl_arr.push_back((nlohmann::json)(std::string)(*board_pair->second));
					}
				}

				std::string dump = tl_arr.dump();

				//check if the last element is a nullptr
 				if (tl_arr.size() > 0 && tl_arr.begin()[tl_arr.size() - 1].is_null())
				{
					//delete last element
					tl_arr.erase(tl_arr.size() - 1);
				}
				
				//add the array to the json object
				timelines[tag] = tl_arr;
			}

			//find CosmeticTurnOffset - zero offset means that the first element is T=1
			//TODO: check if even or odd
			off_t ctf = _board_list[0]._offset() + 1;

			//only add ctf if it is non zero
			if (ctf != 0)
				timelines["CosmeticTurnOffset"] = ctf;

			return timelines;
		}
		/*
		produce a state from json
		*/
		inline explicit state5d(nlohmann::json json_ob)
		{
			auto form_cb = [](nlohmann::json::const_iterator j_begin, int idx, std::pair<size_t, size_t> chk_dim, const std::string& msg_keyval)
			{
				auto ob = j_begin[idx];
				chessboard* cb;
				if (ob.is_string())
				{
					cb = new chessboard(parse<chessboard>((std::string)j_begin[idx]));
				}
				else if (ob.is_null())
				{
					cb = nullptr;
				}
				else
				{
					//dont know what to do here
					cb = nullptr;
				}
				if (cb == nullptr || chk_dim.first != cb->boardw || chk_dim.second != cb->boardh)
				{
					std::cerr << "dimension mismatch of chessboard at: \n"
						<< "\ttl key val: " << msg_keyval << std::endl
						<< "\tindex val:  " << idx << std::endl;
				}
				return cb;
			};
			auto form_tl = [=](const nlohmann::json& tl_ob, const std::string& key, int cos_offset, std::pair<size_t, size_t> dims)
			{
				//add timeline
				horizontal tl;

				//set cos_offset
				tl.shift_fw(cos_offset);

				for (size_t i = 0; i < tl_ob.size(); i += 2)
				{
					//first timeline is guarenteed to be white
					color_pair col_pair;

					//finds the size of the chessboard each time
					col_pair.first = cb_ptr(form_cb(tl_ob.begin(), i, dims, key));
					if (i + 1 < tl_ob.size())
					{
						col_pair.second = cb_ptr(form_cb(tl_ob.begin(), i + 1, dims, key));
					}
					else
					{
						col_pair.second = nullptr;
					}

					//add the entry
					tl.push_back(col_pair);
				}

				return tl;
			};
			

			int tll = -1;
			int tlu = 1;
			int cos_offset = -1;
			std::string key;

			//0 timeline
			std::pair<size_t, size_t> dims;// = chessboard::parse_board_size((std::string)json_ob["0L"][]);

			//read the cosmetic offset
			if (json_ob.contains("CosmeticTurnOffset"))
			{
				cos_offset = (int)json_ob["CosmeticTurnOffset"] - 1;
			}

			//middles
			//check for even timeline
			if (json_ob.contains("-0L"))
			{
				_is_even = true;
				_board_list.push_front(form_tl(json_ob["-0L"], "-0L", cos_offset, dims));
			}
			else
			{
				_is_even = false; 
			}


			//0L and +0L should be equivalent
			horizontal zerol;
			if (json_ob.contains("0L")) zerol = form_tl(json_ob["0L"], "0L", cos_offset, dims);
			else if (json_ob.contains("+0L")) zerol = form_tl(json_ob["+0L"], "+0L", cos_offset, dims);
			_board_list.push_back(std::move(zerol));

			//lowers
			key = std::to_string(tll) + "L";
			while (json_ob.contains(key))
			{
				const nlohmann::json& tl_ob = json_ob[key];

				horizontal tl = form_tl(tl_ob, key, cos_offset, dims);
				_board_list.push_front(tl);

				tll--;
				key = "-" + std::to_string(tll) + "L";
			}

			//uppers
			key = "+" + std::to_string(tlu) + "L";
			while (json_ob.contains(key))
			{
				const nlohmann::json& tl_ob = json_ob[key];

				horizontal tl = form_tl(tl_ob, key, cos_offset, dims);
				_board_list.push_back(tl);

				tlu++;
				key = "+" + std::to_string(tll) + "L";
			}
		}

		//ctor
		/*
		empty constructor - the 0L timeline must be created by make_timeline_up
		*/
		state5d() : _is_even(false) {}
		state5d(bool is_even) : _is_even(1)
		{

		}
		/*
		initial chessboard, bool, and offset
		*/
		state5d(const chessboard& b, bool white)
		{
			color_pair pair = make_color_pair(new chessboard(b), white);
			horizontal h;
			h.push_back(std::move(pair));
			_board_list.push_front(std::move(h));
		}

		/*
		~state5d()
		{
			//delete all of the nonnull chessboards
			//lol using unique ptrs means that deletion is default
		}
		*/
		friend struct parser<state5d, state5d::PARSE_MODE_PGN5D>;
	};

	//appends to the lookup table
	void populate_lookup_table(const nlohmann::json& init_positions_list)
	{
		for (const nlohmann::json& ob : init_positions_list)
		{
			std::string name = ob["Name"];
			const nlohmann::json position = ob["Timelines"];
			state5d state = state5d(position);
			lookup_table.emplace(name, std::move(state));
		}
	}
	//appends to the lookup table
	void populate_lookup_table(std::istream&& init_positions_list)
	{
		nlohmann::json list;
		init_positions_list >> list;
		populate_lookup_table(list);
	}
}