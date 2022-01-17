#pragma once

#include <deque>

namespace AM_common
{
	template<typename collection_t, typename iter_t>
	struct iter_is_in_bound
	{
		inline bool operator()(collection_t col, iter_t iter)
		{
			return col.begin() <= iter && iter < col.end();
		}
	};

	template<typename collection_t, typename iter_t>
	inline bool is_bounded(collection_t col, iter_t iter)
	{
		return iter_is_in_bound<collection_t, iter_t>()(col, iter);
	}

	/*
	bidirectional vector class
	an offset is used when accessing the elements of the vector
	elements may be accessed using negative indexes
	*/
	template<typename t, typename _alloc = std::allocator<t>>
	class bi_vector
	{
		using collection_t = std::deque<t, _alloc>;

	public:
		using off_t = ptrdiff_t;
		using iterator = typename collection_t::iterator;
		using const_iterator = typename collection_t::const_iterator;

	private:
		off_t _zero_off;
		collection_t _collection_ob;

	public:
		//boundries
		inline bool contains_idx(off_t offset) const 
		{
			return head_idx() <= offset && offset <= tail_idx();
		}
		inline off_t begin_idx() const { return head_idx(); }
		inline off_t end_idx() const { return _collection_ob.size() - _zero_off; }

		//random access
		inline iterator at(off_t index) { return begin() + (index + _zero_off); }
		inline const_iterator at(off_t index) const { return begin() + (index + _zero_off); }
		inline t& get(off_t index) { return *at(index); }
		inline const t& get(off_t index) const { return *at(index); }
		inline t& operator[](off_t index) { return get(index); }
		inline const t& operator[](off_t index) const { return get(index); }

		//iterators
		inline iterator begin() { return _collection_ob.begin(); }
		inline const_iterator begin() const { return _collection_ob.begin(); }
		inline iterator zero() { return begin() + _zero_off; }
		inline const_iterator zero() const { return begin() + _zero_off; }
		inline iterator end() { return _collection_ob.end(); }
		inline const_iterator end() const { return _collection_ob.end(); }

		//expansion
		/*
		adds an element to the front of the deque and adjusts the offset to account for it
		eg. if the first element is currently element zero, the new first element becomes element -1 and can be accessed by 
		operator[](-1)
		*/
		inline void push_front(const t& o) { _collection_ob.push_front(o); _zero_off++; }
		inline void push_front(t&& o) { _collection_ob.push_front(std::move(o)); _zero_off++; }
		inline void push_back(const t& o) { _collection_ob.push_back(o); }
		inline void push_back(t&& o) { _collection_ob.push_back(std::move(o)); }
		/*
		expands the deque to include the index value
		*/
		void expand(off_t index_val)
		{
			if (index_val >= end_idx())
			{
				unsigned int size = index_val + _zero_off + 1;
				//_collection_ob.resize(size);
				while (_collection_ob.size() < size) {
					_collection_ob.emplace_back();
				}
			}
			else if (index_val < begin_idx())
			{
				unsigned int size = end_idx() - index_val;
				int new_off = -index_val;

				//_collection_ob.resize(size);
				while (_collection_ob.size() < size) {
					_collection_ob.emplace_front();
				}

				_zero_off = new_off;
			}
			else 
			{
				return;
			}
		}

		//insert operations...

		//shift
		/*
		shifts elements of the bivector forwards
		*/
		inline void shift_fw(off_t offset) { _zero_off += offset; }
		/*
		shifts elements of the bivector backwords
		*/
		inline void shift_bw(off_t offset) { _zero_off -= offset; }

		//head tail
		/*
		head is the first element
		*/
		off_t head_idx() const { return -_zero_off; }
		/*
		tail is the last element
		*/
		off_t tail_idx() const { return end_idx() - 1; }
		iterator head_ptr() { return _collection_ob.begin(); }
		const_iterator head_ptr() const { return _collection_ob.begin(); }
		iterator tail_ptr() { return --_collection_ob.end(); }
		const_iterator tail_ptr() const { return --_collection_ob.end(); }
		t& head() { return *head_ptr(); }
		const t& head() const { return *head_ptr(); }
		t& tail() { return *tail_ptr(); }
		const t& tail() const { return *tail_ptr(); }

		//get
		inline size_t size() const { return _collection_ob.size(); }

		//direct access
		inline const collection_t& _collection() const { return _collection_ob; }
		inline collection_t& _collection() { return _collection_ob; }
		inline off_t _offset() const { return _zero_off; }

		//ctor
		bi_vector()
		{
			_zero_off = 0;
		}
		bi_vector(std::initializer_list<t> init) : _collection_ob(init), _zero_off(0) {}
		//template<typename collection>
		//bi_vector(collection collection_ob) : _collection_ob(collection_ob.begin(), collection_ob.end()) { }
		template<typename fw_iter>
		bi_vector(fw_iter begin, fw_iter end) : _collection_ob(begin, end) {}
	};
}