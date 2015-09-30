#include "../stick.h"

#include "testbench.h"

using namespace xp;

TESTBENCH()

TEST(can_declare_stick) {
	stick s {"abc"};
	stick b {s, 1, 1};
	VERIFY_EQ(3U, s.size());
	VERIFY_EQ(1U, b.size());
	VERIFY_EQ("b", b);
}

TESTFIXTURE(stick)
