#include "../k_array.h"

#include "testbench.h"

using namespace std;
using namespace xp;

TESTBENCH()

TEST(check_dimension) {
	k_array<int, 3, 3, 3> a;
	std::size_t n = size(a);
	VERIFY(n == 27);
}

TEST(check_k_array) {
	k_array<int, 2, 3, 4> a;
	VERIFY(size(a) == 24);
	VERIFY(std::rank<decltype(a)>::value == 3);

	int iota = 0;
	size_t m = extent<decltype(a), 0>::value;
	VERIFY_EQ(2U, m);
	size_t n = extent<decltype(a), 1>::value;
	VERIFY_EQ(3U, n);
	size_t p = extent<decltype(a), 2>::value;
	VERIFY_EQ(4U, p);
	for (size_t i = 0; i < dim<0>(a); ++i) {
		for (size_t j = 0; j < dim<1>(a); ++j) {
			for (size_t k = 0; k < dim<2>(a); ++k) {
				a[i][j][k] = iota++;
			}
		}
	}

	VERIFY(a.at(0, 0, 0) == 0);
	VERIFY(a.at(1, 2, 3) == 23);
}

TESTFIXTURE(k_array)
