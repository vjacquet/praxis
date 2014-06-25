#include <algorithm>
#include <functional>
#include <vector>

#include "../functional.h"

#include "testbench.h"

using namespace std;
using namespace xp;

struct square {
	int operator()(int x) const {
		return x * x;
	}
};

TESTBENCH()

TEST(check_dereference_unary_function) {
	int x = 5;
	auto op = dereference(square {});

	static_assert(is_same<int, decltype(op)::argument_type>::value, "Argument type should be int");

	VERIFY(op(&x) == 25);
}

TEST(check_dereference_binary_function) {
	int x = 5;
	int y = 12;
	auto op = dereference(std::less<int>());

	VERIFY(op(&x, &y));
}

TEST(check_between) {
	vector<int> v {11, 22, 33, 44, 55};

	auto found = find_if(begin(v), end(v), between(30, 40));
	VERIFY(found != end(v) && *found == 33);
}

TEST(can_memoize) {
	int count = 0;
	function<int(int)> fn = [&count](int val) {++count; return val; };
	auto m = memoize(fn);

	VERIFY(m(1) == 1 && count == 1);
	VERIFY(m(2) == 2 && count == 2);
	VERIFY(m(1) == 1 && count == 2);
}

TESTFIXTURE(functional)
