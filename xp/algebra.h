#ifndef __ALGEBRA_H__
#define __ALGEBRA_H__

#include "functional.h"

#include "fakeconcepts.h"

namespace xp {

	template<typename T, BinaryOperation Plus, BinaryOperation Multiplies>
	struct semiring {
		typedef T value_type;

		T value;

		// conversion
		semiring(const T& v) : value(v) {}

		// Semiregular
		semiring() : value() {}

		semiring(const semiring& x) : value(x.value) {}
		semiring(semiring&& x) noexcept : value(std::move(x.value)) {}

		semiring& operator=(const semiring& x) {
			value = x.value;
			return *this;
		}
		semiring& operator=(semiring&& x) {
			value = std::move(x.value);
			return *this;
		}

		// Regular
		inline friend bool operator==(const semiring& x, const semiring& y) {
			return x.value == y.value;
		}
		inline friend bool operator!=(const semiring& x, const semiring& y) {
			return !(x == y);
		}

		// TotallyOrdered
		inline friend bool operator<(const semiring& x, const semiring& y) {
			return x.value < y.value;
		}
		inline friend bool operator <=(const semiring& x, const semiring& y) {
			return !(y < x);
		}
		inline friend bool operator >(const semiring& x, const semiring& y) {
			return y < x;
		}
		inline friend bool operator >=(const semiring& x, const semiring& y) {
			return !(x < y);
		}

		// arithmetic
		semiring& operator+=(const semiring& x) {
			value = Plus()(value, x.value);
			return *this;
		}
		semiring& operator*=(const semiring& x) {
			value = Multiplies()(value, x.value);
			return *this;
		}
		semiring& operator-=(const semiring& x) {
			value = inverse_operation(Plus())(value, x.value);
			return *this;
		}
		semiring& operator/=(const semiring& x) {
			value = inverse_operation(Multiplies())(value, x.value);
			return *this;
		}

		inline friend semiring operator+(semiring x, const semiring& y) {
			return x += y;
		}
		inline friend semiring operator*(semiring x, const semiring& y) {
			return x *= y;
		}
		inline friend semiring operator-(semiring x, const semiring& y) {
			return x -= y;
		}
		inline friend semiring operator/(semiring x, const semiring& y) {
			return x /= y;
		}
	};

}

#endif __ALGEBRA_H__