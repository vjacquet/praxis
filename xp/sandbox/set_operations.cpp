#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../algorithm.h"
#include "../fakeconcepts.h"
#include "../benchmark.h"
#include "../tests/testbench.h"

// benchmarking responses from SO
// <http://stackoverflow.com/questions/25430093/c-differences-between-2-arrays>

using namespace std;
using namespace std::chrono;
using namespace xp;

namespace {
	template<typename C, class URNG>
	C sample(C& c, URNG&& g) {
		const auto n = c.size();
		const uniform_int_distribution<int> dist(n /4, n / 2);
		C r;
		r.reserve(dist(g));
		shuffle(c.begin(), c.end(), std::forward<URNG>(g));
		copy_n(c.cbegin(), c.capacity(), back_inserter(r));
		return r;
	}
}

typedef void(*scenario_t)(vector<int>& v1, vector<int>& v2);

void scenario1(vector<int>& a, vector<int>& b) {
	typedef vector<int> C;

	sort(a.begin(), a.end());
	sort(b.begin(), b.end());

	C only_a;
	C only_b;
	C both;
	only_a.reserve(a.size());
	only_b.reserve(b.size());
	both.reserve(std::min(a.size(), b.size()));
	auto ita = a.cbegin();
	auto enda = a.cend();
	auto itb = b.cbegin();
	auto endb = b.cend();
	while (ita != enda && itb != endb) {
		if (*ita < *itb) {
			only_a.push_back(*ita);
			++ita;
		} else if (*itb < *ita) {
			only_b.push_back(*itb);
			++itb;
		} else {
			both.push_back(*ita);
			++ita;
			++itb;
		}
	}
	only_a.insert(only_a.end(), ita, enda);
	only_b.insert(only_b.end(), itb, endb);
}

void scenario2(vector<int>& a, vector<int>& b) {
	typedef vector<int> C;
	typedef C::value_type T;
	typedef std::list<T> L;
	L only_a(a.cbegin(), a.cend());
	L only_b;
	L both;
	unordered_map<T, typename L::iterator> m(2 * a.size());
	for (auto i = only_a.begin(); i != only_a.end(); ++i)
		m[*i] = i;
	for (auto i : b) {
		auto ai = m.find(i);
		if (ai == m.end()) {
			only_b.push_back(i);
		} else {
			both.push_back(i);
			only_a.erase(ai->second);
		}
	}
}

TESTBENCH()

TEST(bench_partion_sets) {
	const int attempts = 500;

	vector<int> v(50000);
	iota(begin(v), end(v), 1);

	mt19937 rng {1664};
	auto v1 = sample(v, rng);
	auto v2 = sample(v, rng);

	vector<pair<string, scenario_t>> scenarii {
		make_pair("using sorted vectors", &scenario1),
		make_pair("using unordered_map", &scenario2),
	};

	for (auto& scenario : scenarii) {
		measures<microseconds> m;
		for (int attempt = 0; attempt != attempts; ++attempt) {
			auto a = v1;
			auto b = v2;

			timer<high_resolution_clock> w;
			scenario.second(a, b);
			m += w.elapsed<microseconds>();
		}
		cout << "  " << scenario.first << " took an average of " << m.avg().count() << " us." << endl;
	}
}

TESTFIXTURE(set_operations)
