#include "../math/integer.h"
#include "testbench.h"

using namespace xp;

TESTBENCH()

TEST(can_construct_integer) {
	integer i = 5;

	VERIFY(sign(i) > 0);
}


TEST(can_lshift_natural) {
	using namespace xp;

	natural n = 1;
	n <<= 36;
}

TESTFIXTURE(integer)
