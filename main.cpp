#include <iostream>
#include "parser.hpp"

int main() {
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
