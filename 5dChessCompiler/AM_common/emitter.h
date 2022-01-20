#pragma once

#include "observer.h"

namespace AM_common
{
	//emitter implementation using vector
	template<typename... Ts>
	class emitter_V : emitter<Ts...>
	{
	public:
		typedef typename emitter<Ts...>::observer_t observer_t;

	private:
		std::vector<observer_t*> observers;

	public:
		void subscribe(observer_t* ob) override
		{
			observers.push_back(ob);
		}
		void unsubscribe(observer_t* ob) override
		{
			observers.erase(std::find(observers.begin(), observers.end(), ob));
		}
		void emit(const Ts&... args) override
		{
			for (observer_t* ob : observers)
			{
				(*ob)(args...);
			}
		}
	};

	//vector emitter with observer removal/addition during emission
	//still not thread safe
	//no deletion when unsubscribing
	template<typename... Ts>
	class emitter_VM : public emitter<Ts...>
	{
	public:
		typedef typename emitter<Ts...>::observer_t observer_t;
		struct entry
		{
			bool removal = false;
			observer_t* observer;

			void operator()(Ts... Args)
			{
				(*observer)(Args...);
			}

			entry(observer_t* o) { observer = o; }
		};

	private:
		//true when the emitter is emitting
		bool emission = false;
		//true if the emitter has to do a removal operation after it's emission
		bool removal = false;
		std::vector<entry> entries;
		
		std::vector<observer_t*> entry_buffer;

		//remove removal entries
		void _evict()
		{
			for (size_t i = entries.size() - 1; i < entries.size(); i--)
			{
				if (entries[i].removal)
				{
					entries.erase(entries.begin() + i);
				}
			}
			this->removal = false;
		}
		//move entry buffer into the entries vector
		void _employ()
		{
			for (observer_t* ob : entry_buffer)
			{
				entries.push_back(entry(ob));
			}
			entry_buffer.clear();
		}

	public:
		void subscribe(observer_t* ob) override 
		{
			if (emission)
			{
				entry_buffer.push_back(ob);
			}
			else 
			{
				entries.push_back(entry(ob));
			}
		}
		void unsubscribe(observer_t* ob) override
		{
			auto iter = std::find_if(entries.begin(), entries.end(), [=](const entry& e) { return e.observer == ob; });

			if (emission)
			{
				//set the removal flag
				iter->removal = true;
				this->removal = true;
			}
			else
			{
				//remove the entry
				entries.erase(iter);
			}
		}
		void emit(const Ts&... args) override
		{
			//set the emission flag
			emission = true;
			for (entry& entry : entries)
			{
				entry(args...);
			}

			if (removal) _evict();
			if (!entry_buffer.empty()) _employ();

			//remove the emission flag
			emission = false;
		}
	};

	template<typename TKey, typename... Ts>
	class emitter_map
	{
		//TODO
	};
}