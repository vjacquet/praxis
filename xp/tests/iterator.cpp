#include <vector>

#include "../bag.h"
#include "../iterator.h"

#include "testbench.h"

using namespace std;
using namespace xp;

TESTBENCH()

TEST(check_inserter) {
	bag<int> b {1, 2, 3};

	auto ins = inserter(b);
	*ins++ = 4;
	*ins++ = 5;

	VERIFY(b.size() == 5);
	VERIFY(b[4] == 5);
}

TESTFIXTURE(iterator)
