#include <iostream>
#include "parser.hpp"

int main() {
	std::cout << std::flush;

	char x;

	auto a = parse<char>(doer<char>(
		{
			character('('),
			assign(x, item()),
			character(')'),
			success(x)
		}
	), "(i)");

	std::cout << "[";
	if (a.has_value()) {
		std::cout << "('" << a.value().first << "', '" << a.value().second << "')";
	}
	std::cout << "]";
	return 0;
}
