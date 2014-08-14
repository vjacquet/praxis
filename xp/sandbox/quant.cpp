#include <iterator>
#include <vector>

#include "../fakeconcepts.h"
#include "../tests/testbench.h"

namespace xp {

namespace quant {
	// is Times a concept?
	// if the values are 0, 1, 2, ... then the algo could be optimized...

	template<InputIterator I, typename Times, typename T>
	T pv_discrete(I first, I last, Times t, const T& r) {
		T pv {0};
		while (first != last) {
			pv += *first / pow(T {1} +r, *t++);
		}
		return pv;
	}


} // namespace quant

} // namespace xp

TESTBENCH()

TEST(can_compute_present_value_discrete) {

}

TESTFIXTURE(quand)
