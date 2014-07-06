#include <set>
#include <vector>

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

TEST(check_stride_iterator) {
	vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	stride_iterator<vector<int>::iterator, 2> i = v.begin();
	++i;
	VERIFY(*i == 3);
}

TESTFIXTURE(iterator)
