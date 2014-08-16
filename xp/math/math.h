#ifndef __MATH_H__
#define __MATH_H__

namespace xp {

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
		if (x == y) return T {};
		return x / y;
	}

} // namespace xp

#endif __MATH_H__
