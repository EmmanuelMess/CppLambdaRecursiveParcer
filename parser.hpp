#ifndef CPPPARSERTEST_PARSER_HPP
#define CPPPARSERTEST_PARSER_HPP

#include <vector>
#include <functional>

template<typename T>
using Parser = std::function<std::optional<std::pair<T, std::string>>(std::string)>;

template<typename T>
const Parser<T> failure = [](const std::string& input) {
	return std::optional<std::pair<T, std::string>>();
};

template<typename T>
Parser<T> success(const T& ret) {
	return [&ret](const std::string &input) {
		return std::pair<T, std::string>(ret, input);
	};
}

template<typename ... Ts>
struct final_type {
	static_assert(sizeof...(Ts) > 0);
	using type = typename decltype((std::type_identity<Ts>{}, ...))::type;
};

template<typename T1, typename T2>
Parser<T2> doer_impl(const Parser<T1>& parser1, const Parser<T2>& parser2) {
	return [parser1, parser2](const std::string &s) {
		if(s.empty()) {
			return failure<T2>(s);
		}

		auto f = parser1(s);

		if(!f.has_value()) {
			return failure<T2>(s);
		}

		f = parser2(f.value().second);

		return f;
	};
}

template<typename T1>
static Parser<T1> fold(const Parser<T1>& v){
	return v;
}

template<typename T1, typename... Args, typename std::enable_if_t<(sizeof...(Args) > 0), int> = 0, typename = void>
static Parser<typename final_type<Args...>::type> fold(const Parser<T1>& first, const Parser<Args>& ... args) {
	using Tn = typename final_type<Args...>::type;
	return doer_impl<T1, Tn>(first, fold(args...));
}

template<typename ... Ts>
Parser<typename final_type<Ts...>::type> doer(const Parser<Ts>& ... parser) {
	static_assert(sizeof...(Ts) > 0, "can't have 0 params");

	return fold(parser...);
}

template<typename T>
Parser<T> assign(T& x, const Parser<T>& parser) {
	return [&parser, &x](const std::string& input) {
		auto p = parser(input);
		if(!p.has_value()) {
			return failure<char>(input);
		}
		x = p.value().first;
		return p;
	};
}

Parser<char> item() {
	return [](const std::string &input) {
		if(input.empty()) {
			return failure<char>(input);
		}

		auto t = std::pair<char, std::string>(input[0], input.substr(1));
		return std::optional<std::pair<char, std::string>>(t);
	};
}

template<typename T>
Parser<T> sat(const std::function<bool(T)>& f) {
	return [&f](const std::string& input) {
		char x;

		return doer<T, T>(
			assign(x, item()),
			[&x, &f](const std::string &s) {
				return f(x) ? success<T>(x)(s) : failure<T>(s);
			}
		)(input);
	};
}

Parser<char> character(const char& c) {
	return [&c](const std::string &input) {
		auto f = [&c](char other) {
			return c == other;
		};

		return sat<char>(f)(input);
	};
}


template<typename T>
std::optional<std::pair<char, std::string>> parse(const Parser<T> & parser, const std::string & input) {
	return parser(input);
}

template<typename T>
Parser<T> operator+(const Parser<T>& a, const Parser<T>& b) {
	return [a, b](const std::string & input) {
		auto r = parse(a, input);
		if (!r.has_value()) {
			return parse(b, input);
		} else {
			return r;
		}
	};
}
#endif //CPPPARSERTEST_PARSER_HPP
