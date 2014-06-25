#include <type_traits>

#include "../function_traits.h"

#include "testbench.h"

using namespace std;
using namespace xp;

namespace {

	int fn1(int a) {
		return a;
	}

	int fn2(int a, int b) {
		return a + b;
	}

	struct functor {
		//int operator()() {
		//	return 0;
		//}

		int operator()(int a, int b) {
			return a + b;
		}
	};
}

TESTBENCH()

TEST(check_fn1) {
	using Traits = function_traits<decltype(fn1)>;

	static_assert(Traits::arity == 1, "Arity should be 1");
	static_assert(std::is_same<Traits::return_type, int>::value, "Wrong return type");
	static_assert(std::is_same<Traits::argument<0>::type, int>::value, "Wrong argument type");
}

TEST(check_fn2) {
	using Traits = function_traits<decltype(fn2)>;

	static_assert(Traits::arity == 2, "Arity should be 2");
	static_assert(std::is_same<Traits::return_type, int>::value, "Wrong return type");
	static_assert(std::is_same<Traits::argument<0>::type, int>::value, "Wrong argument type");
	static_assert(std::is_same<Traits::argument<1>::type, int>::value, "Wrong argument type");
}

TEST(check_less) {
	using Traits = function_traits<less<int>>;
	static_assert(Traits::arity == 2, "Arity should be 2");
	static_assert(std::is_same<Traits::return_type, bool>::value, "Return type should be bool");
	static_assert(std::is_same<Traits::argument<0>::type, const int&>::value, "First parameter should be const int&");
	static_assert(std::is_same<Traits::argument<1>::type, const int&>::value, "Second parameter should be const int&");
}

TEST(check_functor) {
	using Traits = function_traits<functor>;
	static_assert(Traits::arity == 2, "Arity should be 2");
	static_assert(std::is_same<Traits::return_type, int>::value, "Return type should be int");
	static_assert(std::is_same<Traits::argument<0>::type, int>::value, "First parameter should be int");
	static_assert(std::is_same<Traits::argument<1>::type, int>::value, "Second parameter should be int");
}

TESTFIXTURE(function)
