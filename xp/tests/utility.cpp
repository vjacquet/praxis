#include "../utility.h"

#include "testbench.h"

using namespace std;
using namespace xp;

struct not_stringizeable {

};

TESTBENCH()

TEST(can_stringize) {
	auto msg = stringize(5);
	VERIFY(msg == "5");

	not_stringizeable s;
	msg = stringize(s);
	VERIFY(msg == typeid(s).name());
}

TESTFIXTURE(utility)
