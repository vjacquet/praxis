#include <algorithm>
#include <memory>
#include <vector>

#include "../bag.h"
#include "../instrumented.h"
#include "testbench.h"

using namespace std;
using namespace xp;

TESTBENCH()

TEST(can_construct) {
	bag<int> b(5);

	VERIFY(b.size() == 5);
	VERIFY(b[2] == 0);
}

TEST(can_copy_construct) {
	bag<int> b(5);
	bag<int> c = b;

	VERIFY(c.size() == 5);
	VERIFY(c[2] == 0);
}

TEST(can_construct_with_value) {
	bag<int> b(5, 1);

	VERIFY(b.size() == 5);
	VERIFY(b[2] == 1);
}

TEST(can_construct_from_iterator_range) {
	vector<int> v = {1, 2, 3};
	bag<int> b(v.begin(), v.end());
}

TEST(can_swap) {
	bag<int> x {1, 2, 3};
	bag<int> y {3, 2, 1};
	swap(x, y);

	VERIFY(x[0] == 3);
	VERIFY(y[0] == 1);
}

TEST(can_insert_with_copy) {
	instrumented_base::reset();

	typedef instrumented<int> value_type;

	bag<value_type> b;
	b.reserve(4);

	value_type v1 {1};
	value_type v2 {2};
	b.insert(v1);
	b.insert(v2);

	VERIFY(b[1].value == 2);
	VERIFY(instrumented_base::counts[instrumented_base::operations::copy_construct] == 2);
}

TEST(can_insert_with_move) {
	instrumented_base::reset();

	typedef instrumented<int> value_type;
	static_assert(std::is_nothrow_move_constructible<value_type>::value, "Instrumented should be nothrow move constructible.");

	bag<value_type> b;
	b.reserve(4);

	b.insert(1);
	b.insert(2);

	VERIFY(b[1].value == 2);
	VERIFY(instrumented_base::counts[instrumented_base::operations::move_construct] == 2);
}

TEST(can_emplace) {
	instrumented_base::reset();

	typedef instrumented<int> value_type;

	bag<value_type> b;
	b.reserve(4);

	b.emplace(1);
	b.emplace(2);

	VERIFY(b[1].value == 2);
	VERIFY(instrumented_base::counts[instrumented_base::operations::construct] == 2);
}

TEST(can_assign_il) {
	bag<int> b = {1, 2};

	VERIFY(b[0] == 1);
	VERIFY(b[1] == 2);
}

TEST(can_erase_empty_range) {
	bag<int> b {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	auto i = b.erase(b.begin() + 2, b.begin() + 2);

	VERIFY(b.size() == 10);
	VERIFY(*i == 2);
}

TEST(can_erase_disjoint) {
	bag<int> b {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	b.erase(b.begin() + 2, b.begin() + 4);
	bag<int> expected {0, 1, 8, 9, 4, 5, 6, 7};
	VERIFY(equal(expected.cbegin(), expected.cend(), b.cbegin()));
}

TEST(can_erase_overlapped) {
	bag<int> b {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	b.erase(b.begin() + 5, b.begin() + 8);
	bag<int> expected {0, 1, 2, 3, 4, 8, 9};
	VERIFY(equal(expected.cbegin(), expected.cend(), b.cbegin()));
}

TEST(check_move_on_resize) {
	instrumented_base::reset();

	typedef instrumented<int> value_type;

	bag<value_type> b {0, 1, 2, 3, 4};
	b.reserve(10);

	VERIFY(instrumented_base::counts[instrumented_base::operations::move_construct] == 5);
}

TEST(check_uninitialize_move) {

	typedef instrumented<int> value_type;

	bag<value_type> b {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	value_type p[10];

	instrumented_base::reset();
	uninitialized_move_n(b.begin(), b.size(), p);
	VERIFY(instrumented_base::counts[instrumented_base::operations::move_construct] == 10);
}

TEST(check_use_allocator) {
	using namespace std;
	
	bool bag_uses_allocator = std::uses_allocator<bag<int>, allocator<int>>::value;
	VERIFY(bag_uses_allocator);

	bool instrumented_uses_allocator = std::uses_allocator<instrumented<int>, allocator<int>> ::value;
	VERIFY(!instrumented_uses_allocator);
}

TESTFIXTURE(bag)
