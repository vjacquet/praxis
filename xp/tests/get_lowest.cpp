// Cf. question <http://stackoverflow.com/questions/19772180/efficient-argmin-in-c11>

#include <cassert>

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <utility>
#include <vector>

#include "../algorithm.h"
#include "../benchmark.h"
#include "testbench.h"

using namespace std;
using namespace std::chrono;
using namespace xp;

namespace {
	int newid() {
		static int id = 0;
		return ++id;
	}

	struct Foo {
		mutable int called;
		mutable double value;
		const int id;


		Foo(double val) : value(val), id(newid()), called(0) {}

		double bar() const {
			called++;
			return value;
		}

		void reset() {
			called = 0;
		}
	};

	const Foo& get_lowest_naive(const vector<Foo>& foos) {
		assert(!foos.empty());

		double lowest_so_far = numeric_limits<double>::max();
		vector<Foo>::const_iterator best;
		for (vector<Foo>::const_iterator i = foos.cbegin(); i != foos.cend(); i++) {
			const double curr_val = i->bar();
			if (curr_val < lowest_so_far) {
				best = i;
				lowest_so_far = curr_val;
			}
		}

		return *best;
	}

	const Foo& get_lowest_wierd(const vector<Foo>& foos) {
		struct predicate {
			double lowest;
			predicate(const Foo& first) : lowest(first.bar()) {}
			bool operator()(const Foo& x, const Foo&) {
				auto val = x.bar();
				if (val < lowest) {
					lowest = val;
					return true;
				}
				return false;
			}
		};

		// /!\ must be compiled in Release under Visual Studio. 
		// Sanity checks may fail in Debug mode.
		assert(!foos.empty());
		return *min_element(foos.cbegin(), foos.cend(), predicate(foos.front()));
	}

	const Foo& get_lowest(const vector<Foo>& foos) {
		assert(!foos.empty());
		return *min_cost_element(foos.begin(), foos.end(), mem_fn(&Foo::bar));
	}


	Foo const& get_lowest_stackoverflow(const std::vector<Foo> &foos) {
		typedef decltype(foos[0].bar()) BarVal;

		std::vector<BarVal> barValues;
		barValues.reserve(foos.size());

		std::transform(begin(foos), end(foos), std::back_inserter(barValues), [](Foo const& f) {
			return f.bar();
		});

		auto barPos = std::min_element(begin(barValues), end(barValues));
		auto fooPos = begin(foos) + std::distance(begin(barValues), barPos);
		return *fooPos;
	}

	typedef const Foo& (*scenario_t)(const vector<Foo>& foos);

	vector<Foo> make_sample() {
		auto random = bind(uniform_real_distribution<double>(), mt19937(1664));
		int sample_size = 100000;
		vector<Foo> v;
		v.reserve(sample_size);
		while (sample_size != 0) {
			v.emplace_back(random());
			--sample_size;
		}

		return v;
	}

	vector<Foo> Sample = make_sample();
}

TESTBENCH()

TEST(get_lowest_naive) {
	auto& v = Sample;
	for_each(v.begin(), v.end(), mem_fn(&Foo::reset));

	const auto& lowest = get_lowest(v);
	VERIFY(all_of(v.begin(), v.end(), [](const Foo& foo) { return foo.called == 1; }));

	cout << "  lowest: " << lowest.id << endl;
}

TEST(get_lowest_wierd) {
#ifdef _DEBUG
	SKIP(true)
#endif

		auto& v = Sample;
	for_each(v.begin(), v.end(), mem_fn(&Foo::reset));

	const auto& lowest = get_lowest(v);
	VERIFY(all_of(v.begin(), v.end(), [](const Foo& foo) { return foo.called == 1; }));

	cout << "  lowest: " << lowest.id << endl;
}

TEST(check_get_lowest) {
	auto& v = Sample;
	for_each(v.begin(), v.end(), mem_fn(&Foo::reset));

	const auto& lowest = get_lowest(v);
	VERIFY(all_of(v.begin(), v.end(), [](const Foo& foo) { return foo.called == 1; }));

	cout << "  lowest: " << lowest.id << endl;
}

TEST(check_get_lowest_stackoverflow) {
	auto& v = Sample;
	for_each(v.begin(), v.end(), mem_fn(&Foo::reset));

	const auto& lowest = get_lowest_stackoverflow(v);
	VERIFY(all_of(v.begin(), v.end(), [](const Foo& foo) { return foo.called == 1; }));

	cout << "  lowest: " << lowest.id << endl;
}

TEST(bench_get_lowest) {
	const int attempts = 20000;

	vector<pair<string, scenario_t>> scenarii {
		make_pair("get_lowest_stackoverflow", &get_lowest_stackoverflow),
		make_pair("get_lowest", &get_lowest),
	};

	for (auto& scenario : scenarii) {
		mesures<microseconds> m;
		for (int attempt = 0; attempt != attempts; ++attempt) {
			stopwatch<high_resolution_clock> w;
			auto result = scenario.second(Sample);
			m += w.elapsed<microseconds>();
		}
		cout << "  " << scenario.first << " took an average of " << m.avg().count() << " us." << endl;
	}
}

TESTFIXTURE(get_lowest_fixture)