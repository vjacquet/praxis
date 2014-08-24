
#include "../tests/testbench.h"

namespace xp {

	namespace units {

		struct unit {

		};

		template<typename U, typename T = double>
		struct quantity {
			typedef T value_type;
			typedef U unit_type;

			T value;

			// conversion
			template <typename V>
			explicit quantity(const V& v) : value {v} {}

			// Semiregular
			quantity() {}

			quantity(const quantity& x) : value(x.value) {}
			quantity(quantity&& x) : value(std::move(x.value)) {}

			~quantity() {}

			quantity& operator=(const quantity& x) {
				value = x.value;
				return *this;
			}
			quantity& operator=(quantity&& x) {
				value = std::move(x.value);
				return *this;
			}

			// Regular
			inline friend bool operator==(const quantity& x, const quantity& y) {
				return x.value == y.value;
			}
			inline friend bool operator!=(const quantity& x, const quantity& y) { return !(x == y); }

			// TotallyOrdered
			inline friend bool operator<(const quantity& x, const quantity& y) {
				return x.value < y.value;
			}
			inline friend bool operator <=(const quantity& x, const quantity& y) { return !(y < x); }
			inline friend bool operator >(const quantity& x, const quantity& y) { return y < x; }
			inline friend bool operator >=(const quantity& x, const quantity& y) { return !(x < y); }

			// Arithmetics

		};

	}

}

TESTBENCH()

TEST(can_declare_quanity) {

}

TESTFIXTURE(units)
