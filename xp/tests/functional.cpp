#include <functional>

#include "../functional.h"

#include "testbench.h"

using namespace std;
using namespace xp;

TESTBENCH()

struct square {
	int operator()(int x) const {
		return x * x;
	}
};

TEST(check_dereference_unary_function) {
	int x = 5;
	auto op = dereference(square {});

	VERIFY(op(&x) == 25);
}

TEST(check_dereference_binary_function) {
	int x = 5;
	int y = 12;
	auto op = dereference(std::less<int>());

	VERIFY(op(&x, &y));
}

TESTFIXTURE(functional)
