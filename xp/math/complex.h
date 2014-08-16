#ifndef __COMPLEX_H__
#define __COMPLEX_H__

#include <tuple>
#include <utility>

namespace xp {

	template<typename T>
	class complex {
		T re;
		T im;

		complex(T r, T i) : re(r), im(i) {}

	public:
		typedef T value_type;

		// ctor
		complex() = default;

		friend complex cartesian(T r, T i) {
			return {r, i};
		}

		complex(complex&& x) : re(std::move(x.re)), im(std::move(x.im)) {}
		complex& operator=(complex&& x) {
			re = std::move(x.re);
			im = std::move(x.im);
			return *this;
		}

		complex(const complex& x) = default;
		complex& operator=(const complex& x) = default;

		// conversion
		complex(T x) : re(x), im() {}
		complex& operator=(const T& x) {
			re = x;
			im = T();
			return *this;
		}

		template<typename U>
		complex& operator=(const complex<U>& x) {
			re = Re(x);
			im = Im(x);
			return *this;
		}

		// arithmetic operators
		friend complex operator-(const complex& x) {
			return {-x.re, -x.im};
		}

		template<typename U>
		complex& operator+=(const complex<U>& x) {
			re += Re(x);
			im += Im(x);
			return *this;
		}
		template<typename U>
		complex& operator+=(const U& x) {
			re += x;
			return *this;
		}
		friend complex operator+(const complex& x, const complex& y) {
			return complex {x} += y;
		}
		friend complex operator+(const complex& x, const T& y) {
			return complex {x} += y;
		}
		friend complex operator+(const T& x, const complex& y) {
			return y + x; // commutative
		}

		template<typename U>
		complex& operator-=(const complex<U>& x) {
			re -= Re(x);
			im -= Im(x);
			return *this;
		}
		template<typename U>
		complex& operator-=(const U& x) {
			re -= x;
			return *this;
		}
		friend complex operator-(const complex& x, const complex& y) {
			return complex {x} -= y;
		}
		friend complex operator-(const complex& x, const T& y) {
			return complex {x} -= y;
		}
		friend complex operator-(const T& x, const complex& y) {
			return y - x; // commutative
		}

		template<typename U>
		complex& operator*=(const complex<U>& x) {
			auto r = Re(x), i = Im(x);
			std::tie(re, im) = std::make_pair(re*r - im*i, im*r + re*i);
			return *this;
		}
		template<typename U>
		complex& operator*=(const U& x) {
			re *= x;
			im *= x;
			return *this;
		}
		friend complex operator*(const complex& x, const complex& y) {
			return complex {x} *= y;
		}
		friend complex operator*(const complex& x, const T& y) {
			return complex {x} *= y;
		}
		friend complex operator*(const T& x, const complex& y) {
			return y * x; // commutative
		}

		template<typename U>
		complex& operator/=(const complex<U>& x) {
			auto r = Re(x), i = Im(x);
			auto d = r*r + i*i;
			std::tie(re, im) = std::make_pair((re*r + im*i) / d, (im*r - re*i) / d);
			return *this;
		}
		template<typename U>
		complex& operator/=(const U& x) {
			re /= x;
			im /= x;
			return *this;
		}
		friend complex operator/(const complex& x, const complex& y) {
			return complex {x} /= y;
		}
		friend complex operator/(const complex& x, const T& y) {
			return complex {x} /= y;
		}
		friend complex operator/(const T& x, const complex& y) {
			auto r = Re(y), i = Im(y);
			auto d = r*r + i*i;
			return {x*r / d, -x*i / d};
		}

		// semiregular 
		friend bool operator ==(const complex& x, const complex& y) {
			return x.re == y.re && x.im == re.im;
		}
		friend bool operator !=(const complex& x, const complex& y) {
			return !(x == y)
		}

		// totally ordered is not applicable
		// see <http://en.wikipedia.org/wiki/Complex_number

		// accessor
		friend T& Re(complex& x) { return x.re; }
		friend const T& Re(const complex& x) { return x.re; }
		friend T& Im(complex& x) { return x.im; }
		friend const T& Im(const complex& x) { return x.im; }

		friend complex conjugate(const complex& x) {
			return {x.re, -x.im};
		}
		friend complex inverse(const complex& x) {
			auto d = r*r + i*i;
			return {x.re/d, -x.im/d};
		}
	};

	template<typename T>
	T Re(T&& x) { return std::forward<T>(x); }
	template<typename T>
	T Im(T) { return T {}; }

	template<typename T>
	T abs(const complex<T>& x) {
		auto r = Re(x), i = Im(x);
		return sqrt(r*r + i*i);
	}

}

#endif __COMPLEX_H__
