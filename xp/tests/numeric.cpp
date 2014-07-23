#include <algorithm>
#include <array>
#include <chrono>
#include <functional>
#include <iterator>
#include <numeric>
#include <vector>

#include "testbench.h"
#include "../numeric.h"
#include "../benchmark.h"

using namespace std;
using namespace std::chrono;
using namespace xp;

TESTBENCH()

TEST(check_inner_product_n_nonempty) {
	vector<int> v {1, 2, 3};
	int r1 = inner_product_n(v.begin(), v.begin(), v.size(), 0);
	int r2 = inner_product_n_nonempty(v.begin(), v.begin(), v.size());
	VERIFY(r1 == 14);
	VERIFY(r2 == 14);
}

TEST(accumulate_n_nonempty) {
	vector<int> v {1, 2, 3};
	int r1 = accumulate_n(v.begin(), v.size(), 0);
	int r2 = accumulate_n_nonempty(v.begin(), v.size());
	VERIFY(r1 == 6);
	VERIFY(r2 == 6);
}

TEST(adjacent_difference_n) {
	vector<int> v {1, 2, 3};
	vector<int> actual(3);
	adjacent_difference_n(v.begin(), v.size(), actual.begin());

	vector<int> expected(3);
	adjacent_difference(v.begin(), v.end(), expected.begin());
	VERIFY(actual == expected);
}

TEST(partial_sum_n_nonempty) {
	vector<int> v {1, 2, 3};
	vector<int> o {0, 0, 0};
	partial_sum_n_nonempty(v.begin(), v.size(), o.begin());
	VERIFY(o[0] == 1);
	VERIFY(o[1] == 3);
	VERIFY(o[2] == 6);
}

template<typename T>
struct my_iota_generator {
	T v;
	my_iota_generator(T val) : v(val) {}
	T operator()() {
		return v++;
	}
};

TEST(check_iota_n) {
	using namespace std;
	using xp::iota_n;

	vector<int> v;
	iota_n(back_inserter(v), 10, 0);

	VERIFY(v.size() == 10);
	VERIFY(v[5] == 5);
}

TEST(check_iota_n_with_array) {
	using namespace std;
	using xp::iota_n;

	array<int, 10> v;
	iota_n(begin(v), 10, 0);

	VERIFY(v[5] == 5);
}

TEST(check_reverse_iota_n_with_array) {
	using namespace std;
	using xp::iota_n;

	array<int, 10> v;
	reverse_iota_n(begin(v), 10, 10);

	VERIFY(v[5] == 5);
}

TEST(bench_generic_iota_generator) {
	using namespace std;
	using xp::iota_n;

	const int N = 100000;
	const int attempts = 20000;
	vector<int> v(N);

	vector<pair<string, function<void()>>> scenarii {
		{"iota", [&]() { iota(v.begin(), v.end(), 0); }},
		{"generate", [&]() { generate(v.begin(), v.end(), my_iota_generator<int>{0}); }},
		{"xp::iota_n", [&]() { iota_n(v.begin(), N, 0); }},
		{"generate_n", [&]() { generate_n(v.begin(), N, my_iota_generator<int>{0}); }},
	};

	for (auto& scenario : scenarii) {
		measures<microseconds> m;
		for (int attempt = 0; attempt != attempts; ++attempt) {
			timer<high_resolution_clock> w;
			scenario.second();
			m += w.elapsed<microseconds>();
		}
		cout << "  " << scenario.first << " took an average of " << m.avg().count() << " us." << endl;
	}

}

TEST(check_random_iota) {
	using xp::random_iota;

	const int N = 100;

	vector<int> v1(N);
	random_iota(v1.begin(), v1.end(), 0);

	vector<int> v2(N);
	random_iota(v2.begin(), v2.end(), 0);

	VERIFY(v1 != v2);
}

TESTFIXTURE(numeric)
