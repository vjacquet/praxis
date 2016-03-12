#include <type_traits>
#include <utility>

#include "../fakeconcepts.h"
#include "../tests/testbench.h"

namespace xp {
	namespace details {
		// this should be used to check operators but it doesn't work correctly in VS 2015 Update 1

		template<typename, class = void>
		struct can_make_addable : std::false_type { };
		template<typename T>
		struct can_make_addable<T, std::void_t<decltype(std::declval<T&>() += std::declval<T const&>())>> : std::true_type { };

		template<typename, class = void>
		struct can_make_substractable : std::false_type { };
		template<typename T>
		struct can_make_substractable<T, std::void_t<decltype(std::declval<T&>() -= std::declval<T const&>())>> : std::true_type { };

		template<typename, class = void>
		struct can_make_multipliable : std::false_type { };
		template<typename T>
		struct can_make_multipliable<T, std::void_t<decltype(std::declval<T&>() *= std::declval<T const&>())>> : std::true_type { };

		template<typename, class = void>
		struct can_make_dividable : std::false_type { };
		template<typename T>
		struct can_make_dividable<T, std::void_t<decltype(std::declval<T&>() /= std::declval<T const&>())>> : std::true_type { };

		template<typename, class = void>
		struct can_make_modable : std::false_type { };
		template<typename T>
		struct can_make_modable<T, std::void_t<decltype(std::declval<T&>() %= std::declval<T const&>())>> : std::true_type { };
	}

	namespace op {
		template<typename T>
		struct arithmetics {
			friend auto operator + (T const& x, T const& y) {
				auto t{ x };
				t += y;
				return t;
			}

			friend auto operator - (T const& x, T const& y) {
				auto t{ x };
				t -= y;
				return t;
			}

			friend auto operator * (T const& x, T const& y) {
				auto t{ x };
				t *= y;
				return t;
			}

			friend auto operator / (T const& x, T const& y) {
				auto t{ x };
				t /= y;
				return t;
			}

			friend auto operator % (T const& x, T const& y) {
				auto t{ x };
				t %= y;
				return t;
			}
		};

		template<typename T>
		struct equality_comparable {
			friend auto operator !=(T const& x, T const& y) {
				return !(x == y);
			}
		};

		template<typename T>
		struct equivalence_comparable {
			friend auto operator ==(T const& x, T const& y) {
				return !(x != y);
			}
			friend auto operator !=(T const& x, T const& y) {
				return x < y || y < x;
			}
		};

		template<typename T>
		struct totally_ordered {
			friend auto operator >(T const& x, T const& y) {
				return y < x;
			}
			friend auto operator <=(T const& x, T const& y) {
				return !(y < x);
			}
			friend auto operator >=(T const& x, T const& y) {
				return !(x < y);
			}
		};

		template<typename T>
		struct partially_ordered {
			friend auto operator >(T const& x, T const& y) {
				return y < x;
			}
			friend auto operator <=(T const& x, T const& y) {
				return x < y || x == y;
			}
			friend auto operator >=(T const& x, T const& y) {
				return y < x || x == y;
			}
		};
	}

	class box : op::arithmetics<box>, op::equality_comparable<box>, op::totally_ordered<box> {
		int i;
	public:
		box() :i{} {}
		explicit box(int i) :i(i) {}

		friend bool operator ==(box const& x, box const& y) {
			return x.i == y.i;
		}

		friend bool operator <(box const& x, box const& y) {
			return x.i < y.i;
		}

		box& operator += (box const& x) {
			i += x.i;
			return *this;
		}
		box& operator -= (box const& x) {
			i -= x.i;
			return *this;
		}
		box& operator *= (box const& x) {
			i *= x.i;
			return *this;
		}
		box& operator /= (box const& x) {
			i /= x.i;
			return *this;
		}
		box& operator %= (box const& x) {
			i %= x.i;
			return *this;
		}
	};

} // namespace xp

TESTBENCH()

TEST(can_add) {
	auto one = xp::box{ 1 };
	auto two = one + one;
	auto four = two * two;
}

TEST(can_compare) {
	auto one = xp::box{ 1 };
	auto two = xp::box{ 2 };
	VERIFY(one < two);
}

TESTFIXTURE(operators)
