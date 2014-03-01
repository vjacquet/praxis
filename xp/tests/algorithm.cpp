#include <exception>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "../algorithm.h"
#include "testbench.h"

using namespace std;
using namespace xp;

struct instrumented {
	int val;
	int& count;

	instrumented(int& count, int val) : val(val), count(count) {}
	bool operator() (int other) const {
		++count;
		return val == other;
	}
};

TESTBENCH()

TEST(check_find_backwards) {
	vector<int> v{0, 11, 22, 33, 44, 55, 66, 77, 88, 99};

	auto found = find_backwards(v.cbegin(), v.cend(), 44);
	auto d = distance(v.cbegin(), found);
	VERIFY(d == 4);

	auto not_found = find_backwards(v.cbegin(), v.cend(), 60);
	VERIFY(not_found == v.cend());
}

TEST(check_find_with_hint) {
	using namespace std;
	vector<int> v{0, 11, 22, 33, 44, 55, 66, 77, 88, 99};

	auto found = find(v.cbegin(), v.cend(), v.cbegin() + 5, 44);
	auto d = distance(v.cbegin(), found);
	VERIFY(d == 4);

	auto not_found = find(v.cbegin(), v.cend(), v.cbegin() + 5, 60);
	VERIFY(not_found == v.cend());

	not_found = find(v.cbegin(), v.cend(), v.cbegin() + 3, 60);
	VERIFY(not_found == v.cend());
}

TEST(check_find_if_with_hint) {
	using namespace std;
	vector<int> v{0, 11, 22, 33, 44, 55, 66, 77, 88, 99};

	int count = 0;
	instrumented pred(count, 44);
	auto found = find_if(v.cbegin(), v.cend(), v.cbegin() + 5, pred);
	auto d = distance(v.cbegin(), found);
	VERIFY(d == 4);
	VERIFY(count < 4);

	count = 0;
	instrumented bad(count, 60);
	auto not_found = find_if(v.cbegin(), v.cend(), v.cbegin() + 5, bad);
	VERIFY(not_found == v.cend());
	VERIFY(count == 10);

	count = 0;
	not_found = find_if(v.cbegin(), v.cend(), v.cbegin() + 3, bad);
	VERIFY(not_found == v.cend());
	VERIFY(count == 10);
}

TEST(check_tighten) {
	unsigned char lb = 5;
	unsigned char hb = 25;

	auto result = tighten(70, lb, hb);
	VERIFY(typeid(result) == typeid(lb));
	VERIFY(result == 25);
}

TEST(check_tighten_with_overflow) {
	auto result = tighten<unsigned char>(300, 100, 150);
	VERIFY(typeid(result) == typeid(unsigned char));
	VERIFY(result == 150);
}

TESTFIXTURE(algorithm_fixture)
