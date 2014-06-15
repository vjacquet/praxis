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
		mesures<microseconds> m;
		for (int attempt = 0; attempt != attempts; ++attempt) {
			timer<high_resolution_clock> w;
			scenario.second();
			m += w.elapsed<microseconds>();
		}
		cout << "  " << scenario.first << " took an average of " << m.avg().count() << " us." << endl;
	}

}

TESTFIXTURE(numeric)
