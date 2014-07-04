#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

#include "../functional.h"

#include "testbench.h"

//using namespace std;
using namespace xp;

struct square {
	int operator()(int x) const {
		return x * x;
	}
};

struct barrier {
	int val;

	barrier(int val) : val(val){ };

	friend bool operator==(const barrier& x, const barrier& y) {
		return x.val == y.val;
	}
	friend bool operator!=(const barrier& x, const barrier& y) {
		return !(x == y);
	}
};

TESTBENCH()

TEST(check_operators) {
	std::plus<int> op;
	int a = 1;
	int b = 1;
	int c = op(a, b);
	VERIFY(c == 2);
}

TEST(check_power) {
	VERIFY(power(2, 4) == 16);
}

TEST(check_negative_power) {
	double r = power(2.0, -2);
	VERIFY(r == 0.25); 
}

TEST(check_dereference_unary_function) {
	using namespace std;

	int x = 5;
	auto op = dereference(square {});

	static_assert(is_same<int, dereference_function_t<square>::argument_type>::value, "Argument type should be int");

	VERIFY(op(&x) == 25);
}

TEST(check_dereference_binary_function) {
	int x = 5;
	int y = 12;
	auto op = dereference(std::less<int>());

	VERIFY(op(&x, &y));
}

TEST(check_between) {
	using namespace std;

	vector<int> v {11, 22, 33, 44, 55};

	auto found = find_if(begin(v), end(v), between(30, 40));
	VERIFY(found != end(v) && *found == 33);
}

TEST(can_memoize_with_default_constructible_result) {
	using namespace std;

	int count = 0;
	function<int(int)> fn = [&count](int val) {++count; return val; };
	auto m = memoize(fn);

	VERIFY(m(1) == 1 && count == 1);
	VERIFY(m(2) == 2 && count == 2);
	VERIFY(m(1) == 1 && count == 2);
}

TEST(can_memoize_with_non_default_constructible_result) {
	using namespace std;

	static_assert(!is_default_constructible<barrier>::value, "Barrier cannot be default constructible.");
	static_assert(!is_trivially_default_constructible<barrier>::value, "Barrier cannot be trivially default constructible.");
	static_assert(!is_nothrow_default_constructible<barrier>::value, "Barrier cannot be nothrow default constructible.");

	int count = 0;
	function<barrier(int)> fn = [&count](int val) {++count; return val; };
	auto m = memoize(fn);

	VERIFY(m(1) == 1 && count == 1);
	VERIFY(m(2) == 2 && count == 2);
	VERIFY(m(1) == 1 && count == 2);
}

TEST(check_negation) {
	using namespace std;

	auto op = odd<int>;
	vector<int> v {1, 2, 3, 4};
	VERIFY(find_if(v.begin(), v.end(), negation(op)) == find_if_not(v.begin(), v.end(), op));
}

TESTFIXTURE(functional)
