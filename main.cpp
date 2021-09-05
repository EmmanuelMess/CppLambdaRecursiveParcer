#include <iostream>
#include <vector>
#include <functional>
#include <rapidcheck.h>

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

void test() {
	rc::check("remove first char",
	          [](const std::string &input) {
		          auto a = parse(doer<char>(
			          {
				          item(),
				          success('c')
			          }
		          ), input);

		          if(input.empty()) {
			          RC_ASSERT(!a.has_value());
		          } else {
			          RC_ASSERT(a.has_value());
			          auto t = std::pair<char, std::string>('c', input.substr(1));
			          RC_ASSERT(a.value() == t);
		          }
	          });
	rc::check("get first char",
	          [](const std::string &input) {
		          RC_PRE(!input.empty());
		          char x;
		          auto a = parse(doer<char>({
			                                    assign<char>(x, item()),
			                                    success(x)
		                                    }),
		                         input);
		          RC_ASSERT(x == input[0]);
	          });
	rc::check("fail and follow execution",
	          [](const std::string &input) {
		          RC_PRE(!input.empty());
		          auto a = parse(failure<char> + item(), input);

		          RC_ASSERT(a.has_value());
		          auto t = std::pair<char, std::string>(input[0], input.substr(1));
		          RC_ASSERT(a.value() == t);
	          });
	rc::check("get 1 char, fail and get n chars",
	          [](const unsigned short &value) {
		          RC_PRE(0 < value);

				  std::string input(value, 'a');
		          std::vector<Parser<char>> v(value, item());
		          auto a = parse(
			          doer<char>({item(), failure<char>, success<char>('x')})
			          + doer<char>({doer<char>(v), success<char>('c')}),
			          input);


		          RC_ASSERT(a.has_value());
		          auto t = std::pair<char, std::string>('c', input.substr(value));
		          RC_ASSERT(a.value() == t);
	          });
	rc::check("multiple assigns",
	          [](const std::string &input) {
		          RC_PRE(2 <= input.size());

		          char x;
		          char y;
		          auto a = parse(doer<char>(
			          {
				          assign<char>(x, item()),
				          assign<char>(y, item()),
				          success('c')
			          }
		          ), input);

		          RC_ASSERT(a.has_value());
		          RC_ASSERT(x == input[0]);
		          RC_ASSERT(y == input[1]);
		          auto t = std::pair<char, std::string>('c', input.substr(2));
		          RC_ASSERT(a.value() == t);
	          });
	/*TODO fix
	rc::check("test character",
	          [](const std::string &input) {
		          char x;

		          auto a = parse<char>(doer<char>(
			          {
				          character('('),
				          assign(x, item()),
				          character(')'),
			          }
		          ), "(i)" + input);

		          RC_ASSERT(a.has_value());
		          RC_ASSERT(x == 'i');
		          auto t = std::pair<char, std::string>(')', "");
		          RC_ASSERT(a.value() == t);
	          });
	*/
}

int main() {
	test();

	std::cout << std::flush;

	char x;
	char y;
	auto a = parse(doer<char>(
		{
			assign<char>(x, item()),
			assign<char>(y, item()),
			success(x)
		}
	), "input");

	std::cout << "[";
	if (a.has_value()) {
		std::cout << "(" << a.value().first << ", " << a.value().second << ")";
	}
	std::cout << "]";
	return 0;
}
