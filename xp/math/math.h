#ifndef __MATH_H__
#define __MATH_H__

#include <utility>
#include <functional>
#include "../fakeconcepts.h"

namespace xp {

	template<typename T, Relation Pred, BinaryOperation Op>
	T remainder(T a, T b, Pred pred, Op op) {
		while (!pred(a, b)) {
			a = op(a, b);
		}
		return a;
	}

	template<EuclideanDomain E>
	E remainder(E a, E b) {
		return remainder(a, b, std::less<E>{}, std::minus<E>{});
	}

	int remainder(int a, int b) {
		return a % b;
	}

	template<EuclideanDomain E>
	E gcd(E a, E b) {
		using std::swap;

		while (b != E(0)) {
			a = remainder(a, b);
			swap(a, b);
		}
		return a;
	}

	// see <http://en.wikipedia.org/wiki/Sign_function>.
	template<typename T>
	int csgn(const T& x, const T& zero) {
		if (x < zero) return -1;
		if (x > zero) return 1;
		return 0;
	}
	template<typename T>
	int csgn(const T& x) {
		return csgn(x, {});
	}

	template<typename T>
	T sgn(const T& x) {
		auto y = abs(x);
		if (x == y) return T{};
		return x / y;
	}

} // namespace xp

#endif __MATH_H__
