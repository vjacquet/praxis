#include "../integer.h"
#include "testbench.h"

using namespace xp;

TESTBENCH()

TEST(can_construct_integer) {
	integer i = 5;

	VERIFY(sign(i) > 0);
}

TESTFIXTURE(integer)
