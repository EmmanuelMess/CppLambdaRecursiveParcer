#include <rapidcheck.h>
#include "parser.hpp"

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
	return 0;
}
