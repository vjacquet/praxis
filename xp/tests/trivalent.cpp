#include "../trivalent.h"

#include "testbench.h"

using namespace xp;

TESTBENCH()

TEST(can_construct_trivalent) {
	trivalent v = true;
	VERIFY(is_true(v));
}

TEST(can_compare) {
	trivalent u;
	trivalent t1 = true;
	trivalent t2 = true;
	trivalent f1 = false;
	trivalent f2 = false;

	VERIFY(t1 == t2);
	VERIFY(f1 == f2);
	VERIFY(t1 != f2);
	VERIFY(f1 != t2);

	VERIFY(f1 != u);
	VERIFY(!(f1 == u));
	VERIFY(t1 != u);
	VERIFY(!(t1 == u));

	// as for NaN, u != u
	VERIFY(!(u == u));
	VERIFY(u != u);
}


TESTFIXTURE(trivalent)
