#ifndef CPPPARSERTEST_PARSER_HPP
#define CPPPARSERTEST_PARSER_HPP

#include <vector>
#include <functional>

template<typename T>
using Parser = std::function<std::optional<std::pair<T, std::string>>(std::string)>;

template<typename T>
using LazyParser = std::function<Parser<T>()>;

template<typename T>
const LazyParser<T> failure = []{
	return [](const std::string& input) {
		return std::optional<std::pair<T, std::string>>();
	};
};

template<typename T>
LazyParser<T> success(const T& ret) {
	return [&ret] {
		return [&ret](const std::string &input) {
			return std::pair<T, std::string>(ret, input);
		};
	};
}

template<typename ... Ts>
struct final_type {
	static_assert(sizeof...(Ts) > 0);
	using type = typename decltype((std::type_identity<Ts>{}, ...))::type;
};

template<typename T1, typename T2>
static LazyParser<T2> doer_impl(const LazyParser<T1>& parser1, const LazyParser<T2>& parser2) {
	return [&parser1, &parser2] {
		return [&parser1, &parser2](const std::string &s) {
			if (s.empty()) {
				return failure<T2>()(s);
			}

			auto f = parser1()(s);

			if (!f.has_value()) {
				return failure<T2>()(s);
			}

			f = parser2()(f.value().second);

			return f;
		};
	};
}

template<typename T1>
static LazyParser<T1> fold(const LazyParser<T1>& v){
	return v;
}

template<typename T1, typename... Args, typename std::enable_if_t<(sizeof...(Args) > 0), int> = 0, typename = void>
static LazyParser<typename final_type<Args...>::type> fold(const LazyParser<T1>& first, const LazyParser<Args>& ... args) {
	using Tn = typename final_type<Args...>::type;
	return doer_impl<T1, Tn>(first, fold(args...));
}

template<typename ... Ts>
LazyParser<typename final_type<Ts...>::type> doer(const LazyParser<Ts>& ... parser) {
	static_assert(sizeof...(Ts) > 0, "can't have 0 params");

	return fold(parser...);
}

template<typename T>
LazyParser<T> assign(T& x, const LazyParser<T>& parser) {
	return [&parser, &x] {
		return [&parser, &x](const std::string& input) {
			auto p = parser()(input);
			if(!p.has_value()) {
				return failure<char>()(input);
			}
			x = p.value().first;
			return p;
		};
	};

}

LazyParser<char> item() {
	return []{
		return [](const std::string &input) {
			if (input.empty()) {
				return failure<char>()(input);
			}

			auto t = std::pair<char, std::string>(input[0], input.substr(1));
			return std::optional<std::pair<char, std::string>>(t);
		};
	};
}

template<typename T>
LazyParser<T> sat(const std::function<bool(T)>& f) {
	return [&f] {
		return [&f](const std::string &input) {
			char x;

			return doer<T, T>(
				assign(x, item()),
				f(x) ? success<T>(x) : failure<T>
			)()(input);
		};
	};
}

LazyParser<char> character(const char& c) {
	return [&c] {
		return [&c](const std::string &input) {
			auto f = [&c](char other) {
				return c == other;
			};

			return sat<char>(f)()(input);
		};
	};
}


template<typename T>
std::optional<std::pair<char, std::string>> parse(const LazyParser<T> & parser, const std::string & input) {
	return parser()(input);
}

template<typename T>
LazyParser<T> operator+(const LazyParser<T>& a, const LazyParser<T>& b) {
	return [&a, &b] {
		return [&a, &b](const std::string & input) {
			auto r = parse(a, input);
			if (!r.has_value()) {
				return parse(b, input);
			} else {
				return r;
			}
		};
	};
}

template <typename T>
struct y_combinator {
	std::function<LazyParser<T>(const y_combinator<T>&)> f;

	LazyParser<T> operator()() const {
		//TODO make this implicit
		return f(*this);
	}
};

template <typename T>
LazyParser<T> recursiveCombinator(const std::function<LazyParser<T>(const y_combinator<T>&)>& combinator) {
	return y_combinator<char> { .f = combinator }();
}

#endif //CPPPARSERTEST_PARSER_HPP
