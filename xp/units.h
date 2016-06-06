#ifndef __UNITS_H__
#define __UNITS_H__

#include <type_traits>
#include <utility>

namespace xp {

	namespace units {
		namespace si {

			template<int L, int M, int T, int I, int Th, int N, int J>
			struct unit {
				enum { m = L, kg = M, s = T, A = J, K = Th, mol = N, cd = J };
			};

			namespace details {
				template<typename U1, typename U2>
				struct Unit_add {
					using type = unit<U1::m + U2::m, U1::kg + U2::kg, U1::s + U2::s, U1::A + U2::A, U1::K + U2::K, U1::mol + U2::mol, U1::cd + U2::cd>;
				};

				template<typename U1, typename U2>
				struct Unit_subtract {
					using type = unit<U1::m - U2::m, U1::kg - U2::kg, U1::s - U2::s, U1::A - U2::A, U1::K - U2::K, U1::mol - U2::mol, U1::cd - U2::cd>;
				};
			}

			template<typename U1, typename U2>
			using unit_add = typename details::Unit_add<U1, U2>::type;

			template<typename U1, typename U2>
			using unit_subtract = typename details::Unit_subtract<U1, U2>::type;

			using M = unit<1, 0, 0, 0, 0, 0, 0>;
			using Kg = unit<0, 1, 0, 0, 0, 0, 0>;
			using S = unit<0, 0, 1, 0, 0, 0, 0>;

		}
	}

	template<typename U, typename T = double>
	struct quantity {
		typedef T value_type;
		typedef U unit_type;

		value_type val;

		// conversion
		// val{v} doesn't compile on VS2015 (narrowing conversion from int to double!)
		template <typename V>
		constexpr explicit quantity(V&& v) : val(v) {}

		// Semiregular
		quantity() = default;

		quantity(const quantity& x) : val(x.val) {}
		quantity(quantity&& x) : val(std::forward(x.val)) {}

		~quantity() = default;

		quantity& operator=(const quantity& x) {
			val = x.val;
			return *this;
		}
		quantity& operator=(quantity&& x) {
			value = std::move(x.val);
			return *this;
		}

		// Regular
		inline friend bool operator==(const quantity& x, const quantity& y) {
			return x.val == y.val;
		}
		inline friend bool operator!=(const quantity& x, const quantity& y) { return !(x == y); }

		// TotallyOrdered
		inline friend bool operator<(const quantity& x, const quantity& y) {
			return x.val < y.val;
		}
		inline friend bool operator <=(const quantity& x, const quantity& y) { return !(y < x); }
		inline friend bool operator >(const quantity& x, const quantity& y) { return y < x; }
		inline friend bool operator >=(const quantity& x, const quantity& y) { return !(x < y); }
	};

	template<typename U, typename T1, typename T2>
	auto operator +(quantity<U, T1> x, quantity<U, T2> y) -> quantity<U, decltype(x.val + y.val)> {
		return quantity<U, decltype(x.val + y.val)> {x.val + y.val};
	}

	template<typename U, typename T1, typename T2>
	auto operator -(quantity<U, T1> x, quantity<U, T2> y) -> quantity<U, decltype(x.val - y.val)> {
		return quantity<U, decltype(x.val - y.val)> {x.val - y.val};
	}

	template<typename U1, typename T1, typename U2, typename T2>
	auto operator *(quantity<U1, T1> x, quantity<U2, T2> y) -> quantity<units::si::unit_add<U1, U2>, decltype(x.val * y.val)> {
		return quantity<units::si::unit_add<U1, U2>, decltype(x.val * y.val)> {x.val * y.val};
	}

	template<typename U1, typename T1, typename U2, typename T2>
	auto operator /(quantity<U1, T1> x, quantity<U2, T2> y) -> quantity<units::si::unit_subtract<U1, U2>, decltype(x.val / y.val)> {
		return quantity<units::si::unit_subtract<U1, U2>, decltype(x.val / y.val)> {x.val / y.val};
	}

	// constexpr, as in The C++ programming language, 4th edition, p822, now compiles
	// but the type should be long double and a version with unsigned long long should also be provided for 1_km
	inline constexpr auto operator ""_m (long double d) { return quantity<units::si::M> {static_cast<double>(d)}; }
	inline constexpr auto operator ""_km(long double d) { return quantity<units::si::M> {static_cast<double>(d) * 1000}; }
	inline constexpr auto operator ""_cm(long double d) { return quantity<units::si::M> {static_cast<double>(d) / 100}; }
	inline constexpr auto operator ""_mm(long double d) { return quantity<units::si::M> {static_cast<double>(d) / 1000}; }

	inline constexpr auto operator ""_kg(long double d) { return quantity<units::si::Kg> {static_cast<double>(d)}; }
	inline constexpr auto operator ""_cg(long double d) { return quantity<units::si::Kg> {static_cast<double>(d) / 100}; }
	inline constexpr auto operator ""_g (long double d) { return quantity<units::si::Kg> {static_cast<double>(d) / 1000}; }

	inline constexpr auto operator ""_m(unsigned long long d) { return quantity<units::si::M> {static_cast<double>(d)}; }
	inline constexpr auto operator ""_km(unsigned long long d) { return quantity<units::si::M> {static_cast<double>(d) * 1000}; }
	inline constexpr auto operator ""_cm(unsigned long long d) { return quantity<units::si::M> {static_cast<double>(d) / 100}; }
	inline constexpr auto operator ""_mm(unsigned long long d) { return quantity<units::si::M> {static_cast<double>(d) / 1000}; }

	inline constexpr auto operator ""_kg(unsigned long long d) { return quantity<units::si::Kg> {static_cast<double>(d)}; }
	inline constexpr auto operator ""_cg(unsigned long long d) { return quantity<units::si::Kg> {static_cast<double>(d) / 100}; }
	inline constexpr auto operator ""_g(unsigned long long d) { return quantity<units::si::Kg> {static_cast<double>(d) / 1000}; }

}

#endif __UNITS_H__