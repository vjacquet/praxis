#include "../units.h"

#include "testbench.h"

TESTBENCH()

TEST(can_declare_quantity) {
	using namespace xp;
	using namespace xp::units;

	using m  = si::unit<1, 0, 0, 0, 0, 0, 0>;
	//using kg = si::unit<0, 1, 0, 0, 0, 0, 0>;
	//using s  = si::unit<0, 0, 1, 0, 0, 0, 0>;

	quantity<m, int> q {5};
	VERIFY(q.val == 5);
}

TEST(can_add_quantities) {
	using namespace xp;
	using namespace xp::units;

	using m = si::unit<1, 0, 0, 0, 0, 0, 0>;

	quantity<m, int> x {2};
	quantity<m> y {0.5};
	auto z = x + y;
	VERIFY(z.val == 2.5);
}

TEST(can_divide_quantities) {
	using namespace xp;
	using namespace xp::units;

	using m   = si::unit<1, 0, 0, 0, 0, 0, 0>;
	using s   = si::unit<0, 0, 1, 0, 0, 0, 0>;
	using m_s = si::unit<1, 0, -1, 0, 0, 0, 0>;

	quantity<m> d {5};
	quantity<s> t {2};
	quantity<m_s> speed = d / t;
	VERIFY(speed.val == 2.5);
}

TESTFIXTURE(units)
