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
		int operator()() {
			return 0;
		}

		/*int operator()(int a, int b) {
			return a + b;
		}*/
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

//TEST(check_functor) {
//	using Traits = function_traits<functor>;
//
//	static_assert(Traits::arity == 0, "");
//	static_assert(std::is_same<Traits::return_type, int>::value, "");
//	static_assert(std::is_same<Traits::argument<0>::type, int>::value, "");
//	static_assert(std::is_same<Traits::argument<1>::type, int>::value, "");
//}

TESTFIXTURE(function_traits_fixture)
