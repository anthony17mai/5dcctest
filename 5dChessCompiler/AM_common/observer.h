#pragma once
#include <functional>
#include <vector>

namespace AM_common
{
	template<typename... Ts>
	struct observer
	{
		virtual void operator()(const Ts&... args) = 0;
		virtual ~observer() {}
	};

	template<typename... Ts>
	class observable
	{
	public:
		typedef observer<Ts...> observer_t;

		virtual void subscribe(observer_t* ob) = 0;
		virtual void unsubscribe(observer_t* ob) = 0;
		virtual ~observable() {}
	};

	template<typename... Ts>
	class emitter : observable<Ts...>
	{
	public:
		typedef typename observable<Ts...>::observer_t observer_t;

		virtual void emit(const Ts&... args) = 0;
	};

	//not thread safe
	//simple
	template<typename subject_t>
	class subject : public observable<subject_t>
	{
	public:
		typedef observer<subject_t> observer_t;

	private:
		const subject_t* observed;
		std::vector<observer_t*> observers;

	public:
		void listen(observer_t* ob) override
		{
			observers.push_back(ob);
		}
		void remove(observer_t* ob) override
		{
			typename std::vector<observer_t*>::iterator it = std::find(observers.begin(), observers.end(), ob);
			observers.erase(it);
		}
		void update() const
		{
			for (observer_t* ob : observers) 
			{
				(*ob)(*observed);
			}
		}
	};

	/*
	template<typename subject_t>
	class observe_state
	{
		const subject_t* _subject;
		subject<subject_t> _table;

	public:
		void petition_remove()
		{

		}
	};
	*/
}