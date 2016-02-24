#include <iostream>

#include "expr.h"

#include "../tests/testbench.h"

struct var {
	int val;

	var(int x) : val(x) {}
};

auto eval(var v) {
	return v.val;
}

TESTBENCH() 

TEST(can_add) {
	using namespace xp;

	var x = 2;
	var y = 2;
	VERIFY_EQ(4, eval(x + y));
}

TESTFIXTURE(expr)
