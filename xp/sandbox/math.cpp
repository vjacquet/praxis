#include <iostream>

#include "../tests/testbench.h"

namespace xp {

	template<typename T>
	class complex {
		T re;
		T im;

		complex(T r, T i) : re(r), im(i) {}

	public:
		typedef T vaue_type;

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
			return {-x.re, -y.im};
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
			return {re*r - im*i, im*r + re*i};
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
			return {(re*r + im*i) / d, (im*r - re*i) / d};
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

		friend complex conjugate(const complex& x) {
			return {x.re, -x.im};
		}

		// totally ordered is not applicable
		// see <http://en.wikipedia.org/wiki/Complex_number

		// accessor
		friend T& Re(complex& x) { return x.re; }
		friend const T& Re(const complex& x) { return x.re; }
		friend T& Im(complex& x) { return x.im; }
		friend const T& Im(const complex& x) { return x.im; }
	};

	template<typename T>
	const T& Re(const T& x) { return std::forward<T>(x); }
	template<typename T>
	T&& Im(const T& x) { return {}; }

}

TESTBENCH()

TEST(check_default_ctor) {
	using namespace xp;

	complex<double> z;
	std::cout << "  Re(z)=" << Re(z) << ", Im(z)=" << Im(z) << std::endl;
}

TEST(check_conversion_ctor) {
	using namespace xp;

	complex<double> z = 1;
	std::cout << "  Re(z)=" << Re(z) << ", Im(z)=" << Im(z) << std::endl;
}

TEST(can_create_complex) {
	using namespace xp;

	complex<double> x = 5;

	VERIFY_EQ(5, Re(x));
	VERIFY_EQ(0, Im(x));
}

TEST(can_assign_different_complex) {
	using namespace xp;

	complex<double> x = 1;
	complex<float> y = 2;
	x = y;

	VERIFY_EQ(2, Re(x));
}

TEST(check_imaginary_part) {
	using namespace xp;

	auto r = 5.;

	VERIFY_EQ(0, Im(r));
}

TESTFIXTURE(math)