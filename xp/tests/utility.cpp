#include <exception>

#include "../utility.h"

#include "testbench.h"

using namespace std;
using namespace xp;

struct not_stringizeable {
};

void noop() {}

TESTBENCH()

TEST(can_stringize) {
	auto msg = stringize(5);
	VERIFY(msg == "5");

	not_stringizeable s;
	msg = stringize(s);
	VERIFY(msg == typeid(s).name());
}

TEST(check_at_scope_exit) {
	bool is_clean = false;
	try {
		at_scope_exit cleaup = [&]() { is_clean = true; };

		// nothing to do
		noop();
	}
	catch (...) {
		std::cout << "Caught exception" << endl;
	}
	VERIFY_EQ(true, is_clean);
}

TEST(check_bits) {
	unsigned i = 11;
	VERIFY_EQ(2, bits(i, 2, 2));
}

TESTFIXTURE(utility)
