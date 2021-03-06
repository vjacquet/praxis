#include <iostream>
#include <limits>
#include <stdexcept>

#include "../math/complex.h"
#include "../math/fraction.h"
#include "positive.h"
#include "../tests/testbench.h"

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

TEST(can_compare_fractions) {
	using namespace xp;

	fraction<double> a{ 1, 2 };
	fraction<double> b{ 1, 3 };

	VERIFY(b < a);
}

TEST(check_positive) {
	using namespace xp;

	bool invalid = false;
	try {
		positive<double> p = 1.0;
		auto q = p - 2.0;
	}
	catch (std::invalid_argument& e) {
		invalid = true;
		std::cout << "Caught exception " << e.what() << std::endl;
	}
	VERIFY_EQ(true, invalid);
}

TESTFIXTURE(math)