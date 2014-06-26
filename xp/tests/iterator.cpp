#include <set>

#include "../iterator.h"

#include "testbench.h"

using namespace std;
using namespace xp;

TESTBENCH()

TEST(check_inserter) {
	set<int> s {1, 2, 3};

	auto ins = inserter(s);
	*ins++ = 4;
	*ins++ = 5;

	VERIFY(s.size() == 5);
	VERIFY(s.find(4) != s.end());
}

TESTFIXTURE(iterator)
