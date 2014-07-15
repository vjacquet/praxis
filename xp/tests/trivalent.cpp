#include "../trivalent.h"

#include "testbench.h"

using namespace xp;

TESTBENCH()

TEST(can_construct_trivalent) {
	trivalent v = true;
	VERIFY(is_true(v));
}

TESTFIXTURE(trivalent)
