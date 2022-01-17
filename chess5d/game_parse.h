#pragma once

#include <type_traits>
#include <istream>
#include <string>
#include <sstream>

namespace game
{
	struct INVALID_PARSER {};
	template<typename t>
	struct invalid_parser : std::is_assignable<INVALID_PARSER, t> {};
	struct standard_mode_tag {};

	template<typename dat_t, typename mode_tag = standard_mode_tag>
	struct parser : public INVALID_PARSER {};

	//in general, the output of a writer when parsed should produce (with the same context) the same object
	template<typename ob_t, typename mode_tag = standard_mode_tag>
	struct writer
	{
		//default writer
		void operator()(std::ostream& out, const ob_t& ob) { out << ob; }
	};

	//default container
	template<typename dat_t>
	struct default_container
	{
		bool parse_success;
		dat_t ob;

		//use a default cast operator which allows the container to essentially be transparent
		operator dat_t()
		{
			return ob;
		}
		operator bool() const { return parse_success; }

		//just use default ctor and hope its ok maybe probably not?
		default_container() : ob(), parse_success(false) {}

		//implicit container allows the parser to return a simple value
		default_container(const dat_t& dat) : ob(dat), parse_success(true) {}
		default_container(dat_t&& dat) : ob(dat), parse_success(true) {}

		//syntactic sugar
		default_container(bool b) : ob(), parse_success(b) {}
	};

	//container may be used to add implicit context or to contain metadata
	template<typename dat_t, typename mode_tag = standard_mode_tag>
	struct parse_container : public default_container<dat_t> 
	{
		//inherit ctor
		using default_container<dat_t>::default_container;

		//convert a default container
		parse_container(default_container<dat_t> cont) : default_container<dat_t>(cont) {}
		parse_container() : default_container<dat_t>() {}
	};

	template<typename ob_t, typename mode_tag = standard_mode_tag>
	parse_container<ob_t, mode_tag> parse(std::istream& stream)
	{
		static_assert(!invalid_parser<parse_container<ob_t, mode_tag>>()(), "parser is not declared. did you forget to include a header?");
		static_assert(std::is_assignable<parse_container<ob_t, mode_tag>, decltype(parser<ob_t, mode_tag>()(stream))>()(), "parser must return a type assignable to it's container");
		return parser<ob_t, mode_tag>()(stream);
	}

	template<typename ob_t, typename mode_tag = standard_mode_tag>
	parse_container<ob_t, mode_tag> parse(std::string str)
	{
		std::stringstream stream(str);
		return parse<ob_t, mode_tag>(stream);
	}

	template<typename ob_t, typename mode_tag = standard_mode_tag>
	void write(std::ostream& out, const ob_t& ob)
	{
		writer<ob_t, mode_tag>()(out, ob);
	}

	template<typename ob_t, typename mode_tag = standard_mode_tag>
	std::string write(const ob_t& ob)
	{
		std::stringstream str;
		writer<ob_t, mode_tag>()(str, ob);
		std::string out(std::istreambuf_iterator<char>(str), {});
		return out;
	}
}