#include <type_traits>
#include <utility>

#include "../fakeconcepts.h"
#include "../tests/testbench.h"

#define Action typename

//∨
namespace eop {

	template<typename T>
	struct domain_type {
		typedef T type;
	};

	template<typename T>
	struct domain_type<void(T&)> {
		typedef T type;
	};

	template<typename A>
	using Domain = typename domain_type<A>::type;

	template<typename T>
	struct distance_type {
		typedef int type;
	};

	template<typename T>
	struct distance_type<void(T&)> {
		typedef int type;
	};

	template<>
	struct distance_type<long> {
		typedef long type;
	};

	template<typename A>
	using DistanceType = typename distance_type<A>::type;

	// Exercise 2.6 Rewrite all the algorithms in this chapter in terms of actions

	// /!\ The following did not compile :(
	//     void power_unary(Domain<A>& x, N n, A a)
	template<Action A, Integral N, typename D = Domain<A>>
	void power_unary(D& x, N n, A a) {
		// Precondition: n≥0 ∧(∀i∈N), 0<i≤n ⇒ a^i(x) is defined
		while (n != N(0)) {
			--n;
			a(x);
		}
	}

	template<Action A, typename D = Domain<A>>
	DistanceType<A> distance(D& x, const D& y, A a) {
		// Precondition: y is reachable from x under a
		typedef DistanceType<A> N;
		N n(0);
		while (x != y) {
			a(x);
			++n;
		}
		return n;
	}

	template<Action A, Predicate P, typename D = Domain<A>>
	void collision_point(D& x, A a, P p) {
		// Precondition: p(x) ⇔ a(x) is defined
		if (!p(x)) return;
		D slow = x;
		const D& fast = x;
		a(fast);
		while (fast != slow) {
			if (!p(fast)) return;
			a(fast);
			if (!p(fast)) return;
			a(fast);
			a(slow);
		}
		// Postcondition: x is terminal point or collision_point
	}

	template<Action A, Predicate P, typename D = Domain<A>>
	bool terminating(D& x, A a, P p) {
		// Precondition: p(x) ⇔ a(x) is defined
		collision_point(x, a, p);
		return !p(x);
	}

	template<Action A, typename D = Domain<A>>
	void collision_point_nonterminating_orbit(D& x, A a) {
		if (!p(x)) return;
		D slow = x;
		const D& fast = x;
		a(fast);
		while (fast != slow) {
			a(fast);
			a(fast);
			a(slow);
		}
		// Postcondition: x is terminal point or collision_point
	}
}

namespace {
	void twice(int& i) {
		i *= 2;
	}
}

TESTBENCH()

TEST(check_power_unary) {
	using namespace eop;

	static_assert(std::is_same<int, Domain<decltype(twice)>>::value, "Wrong domain type");
	auto n = 8;
	power_unary(n, 4, twice);
	VERIFY_EQ(128, n)
}

TEST(check_distance) {
	using namespace eop;

	static_assert(std::is_same<int, Domain<decltype(twice)>>::value, "Wrong domain type");
	auto x = 8;
	auto n = distance(x, 128, twice);
	VERIFY_EQ(4, n)
}

TESTFIXTURE(eop2)