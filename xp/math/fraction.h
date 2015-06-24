#ifndef __FRACTION_H__
#define __FRACTION_H__

#include "math.h"

namespace xp {

	template<typename T>
	class fraction {
		T n;
		T d;

	public:
		typedef T value_type;

		// accessor
		const T& numerator() const { return n; }
		const T& denominator() const { return d; }

		// ctor
		fraction() = default;
		fraction(T n, T d) : n(n), d(d) {}

		fraction(fraction&& x) : n(std::move(x.n)), d(std::move(x.d)) {}
		fraction& operator=(fraction&& x) {
			n = std::move(x.n);
			d = std::move(x.d);
			return *this;
		}

		fraction(const fraction& x) = default;
		fraction& operator=(const fraction& x) = default;

		// conversion
		fraction(T x) : n(x), d(T {1}) {}
		fraction& operator=(const T& x) {
			n = x;
			d = T {1};
			return *this;
		}

		template<typename U>
		fraction& operator=(const fraction<U>& x) {
			n = numerator(x);
			d = denominator(x);
			return *this;
		}

		// arithmetic operators
		friend fraction operator-(const fraction& x) {
			return {-x.num, x.denom};
		}

		template<typename U>
		fraction& operator+=(const fraction<U>& x) {
			n = n*x.denominator() + x.numerator()*d;
			d *= x.denominator();
			return *this;
		}
		template<typename U>
		fraction& operator+=(const U& x) {
			num += x;
			return *this;
		}
		friend fraction operator+(const fraction& x, const fraction& y) {
			return fraction {x} += y;
		}
		friend fraction operator+(const fraction& x, const T& y) {
			return fraction {x} += y;
		}
		friend fraction operator+(const T& x, const fraction& y) {
			return y + x; // commutative
		}

		template<typename U>
		fraction& operator-=(const fraction<U>& x) {
			n = n*x.denominator() - x.numerator()*d;
			d *= x.denominator();
			return *this;
		}
		template<typename U>
		fraction& operator-=(const U& x) {
			num -= x;
			return *this;
		}
		friend fraction operator-(const fraction& x, const fraction& y) {
			return fraction {x} -= y;
		}
		friend fraction operator-(const fraction& x, const T& y) {
			return fraction {x} -= y;
		}
		friend fraction operator-(const T& x, const fraction& y) {
			return -(y - x);
		}

		template<typename U>
		fraction& operator*=(const fraction<U>& x) {
			auto r = num(x), i = denom(x);
			return {num*r - denom*i, denom*r + num*i};
		}
		template<typename U>
		fraction& operator*=(const U& x) {
			num *= x;
			denom *= x;
			return *this;
		}
		friend fraction operator*(const fraction& x, const fraction& y) {
			return fraction {x} *= y;
		}
		friend fraction operator*(const fraction& x, const T& y) {
			return fraction {x} *= y;
		}
		friend fraction operator*(const T& x, const fraction& y) {
			return y * x; // commutative
		}

		template<typename U>
		fraction& operator/=(const fraction<U>& x) {
			return {n*x.denominator(), d*x.numerator()};
		}
		template<typename U>
		fraction& operator/=(const U& x) {
			d *= x;
			return *this;
		}
		friend fraction operator/(const fraction& x, const fraction& y) {
			return fraction {x} /= y;
		}
		friend fraction operator/(const fraction& x, const T& y) {
			return fraction {x.n, x.d * y};
		}
		friend fraction operator/(const T& x, const fraction& y) {
			return {x*y.d, y.n};
		}

		// semiregular 
		friend bool operator ==(const fraction& x, const fraction& y) {
			return x.n == y.n && x.d == num.d;
		}
		friend bool operator !=(const fraction& x, const fraction& y) {
			return !(x == y)
		}

		// totally ordered
		friend bool operator <(const fraction& x, const fraction& y) {
			return sgn(x - y) < 0; // could be optimized
		}
		inline friend bool operator <=(const fraction& x, const fraction& y) {
			return !(y < x);
		}
		inline friend bool operator >(const fraction& x, const fraction& y) {
			return y < x;
		}
		inline friend bool operator >=(const fraction& x, const fraction& y) {
			return !(x < y);
		}
	};

	template<typename T>
	fraction<T> abs(const fraction<T>& x) {
		return {abs(x.numerator()), abs(x.denominator())};
	}

	// see <http://en.wikipedia.org/wiki/Sign_function>.
	template<typename T>
	int csgn(const fraction<T>& x, const T& zero) {
		return csgn(x.numerator(), zero)*csgn(x.denominator(), zero);
	}
	template<typename T>
	int csgn(const fraction<T>& x) {
		return csgn(x, T{});
	}

	template<typename T>
	T sgn(const fraction<T>& x) {
		return csgn(x) * T {1};
	}

	template<typename T>
	fraction<T> inverse(const fraction<T>& x) {
		return {x.denominator(), x.numerator()};
	}
	template<typename T>
	fraction<T> inverse(const T& x) {
		return {T {1}, x};
	}
}

#endif __FRACTION_H__
