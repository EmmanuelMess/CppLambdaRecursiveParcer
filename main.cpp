#include <iostream>
#include "parser.hpp"

int main() {
	std::cout << std::flush;

	char x;

	auto m = [&x](const y_combinator<char>& conb) {
		return doer(
			character('('),
			conb() + assign(x, item()),
			character(')')
		)
		+ doer(success(x));
	};

	auto fun = recursiveCombinator<char>(m);

	auto a = parse(fun, "(i)");

	std::cout << "[";
	if (a.has_value()) {
		std::cout << "('" << a.value().first << "', '" << a.value().second << "')";
	}
	std::cout << "]";
	return 0;
}
