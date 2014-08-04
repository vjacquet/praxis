#include "../heap.h"

#include "testbench.h"

using namespace std;
using namespace xp;

TESTBENCH()

TEST(can_construct) {
	heap<int> h;

	h.push(5);
	h.push(2);
	h.push(1);
	h.push(9);
	h.push(8);

	VERIFY_EQ(9, h.top()); h.pop();
	VERIFY_EQ(8, h.top()); h.pop();
	VERIFY_EQ(5, h.top()); h.pop();
	VERIFY_EQ(2, h.top()); h.pop();
	VERIFY_EQ(1, h.top()); h.pop();
	VERIFY(h.empty());
}

TESTFIXTURE(heap)