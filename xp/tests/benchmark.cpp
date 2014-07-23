#include <chrono>
#include <iostream>
#include <vector>

#include "../numeric.h"
#include "../benchmark.h"

#include "testbench.h"

using namespace xp;

TESTBENCH()

TEST(can_reset) {
	using namespace std;
	using namespace std::chrono;

	vector<int> v(10000000);

	timer<steady_clock> w;
	random_iota_n(v.begin(), v.size(), 0);
	auto m1 = w.elapsed<nanoseconds>();

	w.reset();
	random_iota_n(v.begin(), v.size() / 2, 0);
	auto m2 = w.elapsed<nanoseconds>();

	VERIFY(m2 < m1);
}

TEST(check_is_semiregular) {
	using namespace std;
	using namespace std::chrono;

	vector<int> v(10000000);

	timer<steady_clock> w1;
	random_iota_n(v.begin(), v.size(), 0);

	timer<steady_clock> w2 = w1;
	random_iota_n(v.begin(), v.size(), 0);

	auto m1 = w1.elapsed<microseconds>();
	auto m2 = w1.elapsed<microseconds>();

	VERIFY(m1 == m2);
	cout << "  m1:" << m1.count() << "us, m2:" << m2.count() << "us" << endl;
}

TEST(check_is_totallyordered) {
	using namespace std;
	using namespace std::chrono;

	vector<int> v(10000000);

	timer<steady_clock> w1;
	random_iota_n(v.begin(), v.size(), 0);

	timer<steady_clock> w2;
	random_iota_n(v.begin(), v.size(), 0);

	VERIFY(w2 < w1);
}

TEST(can_mesure_durations) {
	using namespace std;
	using namespace std::chrono;

	seconds s1(4);
	seconds s2(6);
	seconds s3(8);

	measures<seconds> m;
	m += s1;
	m += s2;
	m += s3;

	VERIFY(m.min() == s1);
	VERIFY(m.avg() == s2);
	VERIFY(m.max() == s3);
}

TESTFIXTURE(benchmark)
