#include <rapidcheck.h>
#include "parser.hpp"

void test() {
	rc::check("remove first char",
	          [](const std::string &input) {
		          auto a = parse(doer(
					  item(),
					  success('c')
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
		          auto a = parse(doer(
					  assign<char>(x, item()),
					  success(x)
					  ),
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
	rc::check("get 1 char, fail and get 5 chars",
	          []() {
		          auto value = *rc::gen::inRange(5, 100);

		          std::string input(value, 'a');

				  auto f = doer(item(), item(), item(), item(), item());

		          auto a = parse(
			          doer(item(), failure<char>, success('x'))
			          + doer(f, success('c')),
			          input);

		          RC_ASSERT(a.has_value());
		          auto t = std::pair<char, std::string>('c', input.substr(5));
		          RC_ASSERT(a.value() == t);
	          });
	rc::check("multiple assigns",
	          [](const std::string &input) {
		          RC_PRE(2 <= input.size());

		          char x;
		          char y;
		          auto a = parse(doer(
				          assign<char>(x, item()),
				          assign<char>(y, item()),
				          success('c')
		          ), input);

		          RC_ASSERT(a.has_value());
		          RC_ASSERT(x == input[0]);
		          RC_ASSERT(y == input[1]);
		          auto t = std::pair<char, std::string>('c', input.substr(2));
		          RC_ASSERT(a.value() == t);
	          });
	rc::check("test character",
	          [](const std::string &input) {
		          char x;

		          auto a = parse<char>(doer<char, char, char, char>(
				          character('('),
				          assign(x, item()),
				          character(')'),
						  success(x)
		          ), "(i)" + input);

		          RC_ASSERT(a.has_value());
		          RC_ASSERT(x == 'i');
		          auto t = std::pair<char, std::string>(x, input);
		          RC_ASSERT(a.value() == t);
	          });
}

int main() {
	test();
	return 0;
}
