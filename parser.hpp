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

template<typename T>
Parser<T> doer(const std::vector<Parser<T>>& parsers) {
	return [parsers](const std::string &s) {
		if(parsers.empty()) {
			return failure<T>(s);
		}

		auto f = parsers.front()(s);

		for (int i = 1; i < parsers.size(); ++i) {
			if(!f.has_value()) {
				return failure<T>(s);
			}

			f = parsers[i](f.value().second);
		}

		return f;
	};
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
	return [](const std::string &s) {
		if(s.empty()) {
			return failure<char>(s);
		}

		return std::optional<std::pair<char, std::string>>(std::pair<char, std::string>(s[0], s.substr(1)));
	};
}

Parser<char> sat(const std::function<bool(char)> f) {
	char x;

	return doer<char>({
		                  assign(x, item()),
		                  [&x, &f](const std::string& s) {
			                  return f(x)? success(x)(s) : failure<char>(s);
		                  }
	                  });
}

Parser<char> character(const char& c) {
	return sat([&c](char other){
		return c == other;
	});
}


template<typename T>
std::optional<std::pair<char, std::string>> parse(const Parser<T> & parser, const std::string & input) {
	return parser(input);
}

template<typename T>
Parser<T> operator+(const Parser<T>& a, const Parser<T>& b) noexcept {
	return [&](const std::string & input) {
		auto r = parse(a, input);
		if (!r.has_value()) {
			return parse(b, input);
		} else {
			return r;
		}
	};
}
#endif //CPPPARSERTEST_PARSER_HPP
