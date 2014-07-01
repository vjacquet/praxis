#include <functional>

#include "../algebra.h"

#include "testbench.h"

using namespace std;
using namespace xp;

TESTBENCH()

TEST(check_boolean_semiring) {
	typedef semiring<bool, logical_or<bool>, logical_and<bool>> boolean;

	boolean a = true;
	boolean b = false;

	VERIFY((a + b) == true);
	VERIFY((a * b) == false);
}

TESTFIXTURE(algebra)