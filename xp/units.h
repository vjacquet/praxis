#ifndef __UNITS_H__
#define __UNITS_H__

#include <utility>

namespace xp { 
	
	namespace units { namespace si {

		template<int L, int M, int T, int I, int Th, int N, int J>
		struct unit {
			enum { m=L, kg=M, s=T, A=J, K=Th, mol=N, cd=J};
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

	} } 

	template<typename U, typename T = double>
	struct quantity {
		typedef T value_type;
		typedef U unit_type;

		value_type val;

		// conversion
		template <typename V>
		explicit quantity(const V& v) : val {v} {}

		// Semiregular
		quantity() {}

		quantity(const quantity& x) : val(x.val) {}
		quantity(quantity&& x) : val(std::move(x.val)) {}

		~quantity() {}

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
	auto operator +(quantity<U, T1> x, quantity<U, T2> y) -> quantity<U, decltype(x.val + y.val)>
	{
		return quantity<U, decltype(x.val + y.val)> {x.val + y.val};
	}

	template<typename U, typename T1, typename T2>
	auto operator -(quantity<U, T1> x, quantity<U, T2> y) -> quantity<U, decltype(x.val - y.val)>
	{
		return quantity<U, decltype(x.val - y.val)> {x.val - y.val};
	}

	template<typename U1, typename T1, typename U2, typename T2>
	auto operator *(quantity<U1, T1> x, quantity<U2, T2> y) -> quantity<units::si::unit_add<U1,U2>, decltype(x.val * y.val)>
	{
		return quantity<units::si::unit_add<U1, U2>, decltype(x.val * y.val)> {x.val * y.val};
	}

	template<typename U1, typename T1, typename U2, typename T2>
	auto operator /(quantity<U1, T1> x, quantity<U2, T2> y) -> quantity<units::si::unit_subtract<U1, U2>, decltype(x.val / y.val)>
	{
		return quantity<units::si::unit_subtract<U1, U2>, decltype(x.val / y.val)> {x.val / y.val};
	}
}

#endif __UNITS_H__