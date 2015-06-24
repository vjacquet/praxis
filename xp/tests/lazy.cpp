#include "../instrumented.h"
#include "../lazy.h"

#include "../tests/testbench.h"

using namespace std;
using namespace xp;

TESTBENCH()

TEST(check_deferred) {
	instrumented_base::reset();
	{
		lazy<instrumented<int>> l {5};
		auto& i = l.get();
	}
	VERIFY_EQ(1, instrumented_base::counts[instrumented_base::construct]);
	VERIFY_EQ(1, instrumented_base::counts[instrumented_base::destruct]);
}

TEST(check_deferred_do_not_instanciate_when_not_called) {
	instrumented_base::reset();
	{
		lazy<instrumented<int>> l {5};
	}
	VERIFY_EQ(0, instrumented_base::counts[instrumented_base::construct]);
	VERIFY_EQ(0, instrumented_base::counts[instrumented_base::destruct]);
}

TEST(check_async) {
	instrumented_base::reset();
	{
		lazy<instrumented<int>> l {std::launch::async, 5};
		auto& i = l.get(); // should not be necessary, async should block.
	}
	VERIFY_EQ(1, instrumented_base::counts[instrumented_base::construct]);
	VERIFY_EQ(1, instrumented_base::counts[instrumented_base::destruct]);
}

TESTFIXTURE(lazy)
