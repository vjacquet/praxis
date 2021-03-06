#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

#include "../functional.h"
#include "../numeric.h"

#include "testbench.h"

//using namespace std;
using namespace xp;

struct empty_t {};

template<typename T, typename Op = std::multiplies<T>>
struct square {
	typedef typename Op::result_type result_type;

	Op op;
	T operator()(T x) const {
		return op(x, x);
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

int select1rst(int x, int /*y*/) {
	return x;
}

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
	auto op = dereference(square<int> {});

	static_assert(is_same<int, dereference_function_t<square<int>>::argument_type>::value, "Argument type should be int");

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
	using xp::negation; // c++ 17 added a negation in the type_traits. Should have been better in another namespace :( !
	using namespace xp::integers;

	auto op = odd<int>;
	vector<int> v {1, 2, 3, 4};
	VERIFY(find_if(v.begin(), v.end(), negation(op)) == find_if_not(v.begin(), v.end(), op));
}

TEST(check_select) {
	using namespace std;
	auto p = make_pair(1, string("one"));

	auto select = select_element<0, decltype(p)>();
	VERIFY_EQ(1, select(p));
}

TEST(check_transpose) {
	using namespace xp;
	auto r = transpose(select1rst)(1, 2);
	VERIFY_EQ(2, r);
}

TEST(check_compose) {
	using namespace xp;
	using namespace std;

	static_assert(function_traits<plus<int>>::arity == 2, "Plus is a BinaryOperation");
	static_assert(function_traits<square<int>>::arity == 1, "Square is a UnaryOperation");

	auto r1 = compose(plus<int>{}, square<int>{})(3, 4);
	VERIFY_EQ(25, r1);

	auto r2 = compose(square<int>{}, plus<int>{})(3, 4);
	VERIFY_EQ(49, r2);
}

TESTFIXTURE(functional)
