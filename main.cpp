#include <iostream>
#include "parser.hpp"

int main() {
	std::cout << std::flush;

	char x;

	Parser<char> f =
		doer(
			character('('),
			f + assign(x, item()),
			character(')')
		)
		+ doer(success(x));

	auto a = parse(f, "(i)");

	std::cout << "[";
	if (a.has_value()) {
		std::cout << "('" << a.value().first << "', '" << a.value().second << "')";
	}
	std::cout << "]";
	return 0;
}
