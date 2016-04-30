#ifndef __POSITIVE_H__
#define __POSITIVE_H__

#include <type_traits>
#include "../fakeconcepts.h"

namespace xp {
	template<typename T>
	class positive {
		T value;
		void ensure_invariant() const {
			if (!(value > T{}))
				throw std::invalid_argument("The value must be positive.");
		}
		positive(T x, std::true_type) : value(x) {
		}

	public:
		typedef T value_type;

		// conversion
		positive(T x) : value(x) {
			ensure_invariant();
		}

		T get() const { return value; }
		explicit operator T() const { return get(); }

		positive() = delete;
		positive(positive const&) noexcept = default;
		positive(positive&&) noexcept = default;

		positive& operator=(positive const& x) = default;

		template<typename U, typename Dummy = std::enable_if<std::is_convertible<U, T>::value>>
		positive(const positive<U>& x) : value(x.get()) {
		}
		template<typename U, typename Dummy = std::enable_if<std::is_convertible<U, T>::value>>
		positive& operator=(const positive<U>& x) {
			value = x.get();
		}

		inline friend auto operator==(positive const& x, positive const& y) {
			return x.value == y.value;
		}
		inline friend auto operator!=(positive const& x, positive const& y) {
			return x.value != y.value;
		}

		// TotallyOrdered
		inline friend auto operator<(positive const& x, positive const& y) {
			return x.value < y.value;
		}
		inline friend auto operator <=(positive const& x, positive const& y) {
			return x.value <= y.value;
		}
		inline friend auto operator >(positive const& x, positive const& y) {
			return x.value > y.value;
		}
		inline friend auto operator >=(positive const& x, positive const& y) {
			return x.value >= y.value;
		}

		// Arithmetics
		inline friend auto operator+(positive const& x, positive const& y) {
			return positive{ x.value + y.value, std::true_type{} };
		}

		inline friend auto operator-(positive const& x, positive const& y) {
			return positive{ x.value - y.value };
		}

		inline friend auto operator*(positive const& x, positive const& y) {
			return positive{ x.value * y.value, std::true_type{} };
		}

		inline friend auto operator/(positive const& x, positive const& y) {
			return positive{ x.value / y.value, std::true_type{} };
		}

		inline friend auto operator%(positive const& x, positive const& y) {
			return positive{ x.value % y.value, std::true_type{} };
		}
		inline friend positive operator-(positive const&) = delete;
	};
}


#endif __POSITIVE_H__