#include <queue>

#include "../heap.h"

#include "../tests/testbench.h"

using namespace std;
using namespace xp;

namespace alt {

	// heap could be replaced by the following using. Please note that the predicate and
	// the container parameters are reversed, as we are more likely to change the predicate
	// than the underlying container.
	template<Semiregular T, Predicate Pred = std::less<T>, typename Cont = std::vector<T>>
	using heap = std::priority_queue<T, Cont, Pred>;

}


TESTBENCH()

TEST(can_construct_heap) {
	xp::heap<int> h;

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


TEST(can_construct_alternate_heap) {
	alt::heap<int> h;

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