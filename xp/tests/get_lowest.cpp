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

	Foo const& get_lowest_naive(const Foo* first, const Foo* last) {
		assert(first != last);

		double lowest_so_far = numeric_limits<double>::max();
		const Foo* best = nullptr;
		for (auto i = first; i != last; i++) {
			const double curr_val = i->bar();
			if (curr_val < lowest_so_far) {
				best = i;
				lowest_so_far = curr_val;
			}
		}

		return *best;
	}

	Foo const& get_lowest_naive_optimized(const Foo* first, const Foo* last) {
		assert(first != last);

		auto best = first;
		double lowest_so_far = first->bar();
		while (++first != last) {
			const double curr_val = first->bar();
			if (curr_val < lowest_so_far) {
				best = first;
				lowest_so_far = curr_val;
			}
		}

		return *best;
	}

	template<ForwardIterator I, Predicate P> inline
		I wierdo_nonempty(I first, I last, P pred)
	{	// find smallest element, using pred
			I found = first;
			for (; ++first != last;)
				if (pred(*first, *found))
					found = first;
			return found;
	}

	template<ForwardIterator I, Function Cost>
	I min_cost_element_wierd(I first, I last, Cost cost) {
		struct pseudo_predicate {
			typedef decltype(*first) T;
			typedef decltype(cost(*first)) C;

			Cost cost;
			C lowest;
			pseudo_predicate(Cost cost, const T& t) : cost(cost), lowest(cost(t)) {}
			bool operator()(const Foo& x, const Foo&) {
				auto val = cost(x);
				if (val < lowest) {
					lowest = val;
					return true;
				}
				return false;
			}
		};
		if (first == last) return last;
		return wierdo_nonempty(first, last, pseudo_predicate(cost, *first));
	}

	Foo const& get_lowest_wrapped_wierd(const Foo* first, const Foo* last) {
		assert(first != last);
		return *min_cost_element_wierd(first, last, [](Foo const& f) { return f.bar(); });
	}

	Foo const& get_lowest_wierd(const Foo* first, const Foo* last) {
		struct pseudo_predicate {
			double lowest;
			pseudo_predicate(const Foo& first) : lowest(first.bar()) {}
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
		assert(first != last);
		// the predicate is not called for the first item
		return *min_element(first, last, pseudo_predicate(*first));
	}

	struct cost {
		double operator()(const Foo& x) {
			return x.bar();
		}
	};

	Foo const& get_lowest(const Foo* first, const Foo* last) {
		assert(first != last);
		return *min_cost_element(first, last, cost());
	}

	Foo const& get_lowest_mem_fn(const Foo* first, const Foo* last) {
		assert(first != last);
		return *min_cost_element(first, last, mem_fn(&Foo::bar));
	}

	Foo const& get_lowest_lambda(const Foo* first, const Foo* last) {
		assert(first != last);
		return *min_cost_element(first, last, [](Foo const& f) { return f.bar(); });
	}

	Foo const& get_lowest_stackoverflow(const Foo* first, const Foo* last) {
		assert(first != last);
		typedef decltype(first->bar()) BarVal;

		std::vector<BarVal> barValues;
		barValues.reserve(last - first);

		std::transform(first, last, std::back_inserter(barValues), [](Foo const& f) {
			return f.bar();
		});

		auto barPos = std::min_element(begin(barValues), end(barValues));
		auto fooPos = first + std::distance(begin(barValues), barPos);
		return *fooPos;
	}

	typedef const Foo& (*scenario_t)(const Foo* first, const Foo* last);

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

TEST(check_get_lowest_naive) {
	auto& v = Sample;
	for_each(v.begin(), v.end(), mem_fn(&Foo::reset));

	auto first = v.data();
	auto last = first + v.size();
	const auto& lowest = get_lowest_naive(first, last);
	VERIFY(all_of(v.begin(), v.end(), [](const Foo& foo) { return foo.called == 1; }));

	cout << "  lowest: " << lowest.id << endl;
}

TEST(check_get_lowest_wierd) {
#ifdef _DEBUG
	SKIP(true)
#endif

	auto& v = Sample;
	for_each(v.begin(), v.end(), mem_fn(&Foo::reset));

	auto first = v.data();
	auto last = first + v.size();
	const auto& lowest = get_lowest_wierd(first, last);
	VERIFY(all_of(v.begin(), v.end(), [](const Foo& foo) { return foo.called == 1; }));

	cout << "  lowest: " << lowest.id << endl;
}

TEST(check_get_lowest) {
	auto& v = Sample;
	for_each(v.begin(), v.end(), mem_fn(&Foo::reset));

	auto first = v.data();
	auto last = first + v.size();
	const auto& lowest = get_lowest(first, last);
	VERIFY(all_of(v.begin(), v.end(), [](const Foo& foo) { return foo.called == 1; }));

	cout << "  lowest: " << lowest.id << endl;
}

TEST(check_get_lowest_stackoverflow) {
	auto& v = Sample;
	for_each(v.begin(), v.end(), mem_fn(&Foo::reset));

	auto first = v.data();
	auto last = first + v.size();
	const auto& lowest = get_lowest_stackoverflow(first, last);
	VERIFY(all_of(v.begin(), v.end(), [](const Foo& foo) { return foo.called == 1; }));

	cout << "  lowest: " << lowest.id << endl;
}

TEST(bench_get_lowest) {
	const int attempts = 20000;

	vector<pair<string, scenario_t>> scenarii {
		make_pair("get_lowest_naive", &get_lowest_naive),
		make_pair("get_lowest_naive_optimized", &get_lowest_naive_optimized),
		make_pair("get_lowest_wierd", &get_lowest_wierd),
		make_pair("get_lowest_wrapped_wierd", &get_lowest_wrapped_wierd),
		make_pair("get_lowest", &get_lowest),
		make_pair("get_lowest_mem_fn", &get_lowest_mem_fn),
		make_pair("get_lowest_lambda", &get_lowest_lambda),
		make_pair("get_lowest_stackoverflow", &get_lowest_stackoverflow),
	};

	for (auto& scenario : scenarii) {
		measures<microseconds> m;
		for (int attempt = 0; attempt != attempts; ++attempt) {
			auto& v = Sample;
			for_each(v.begin(), v.end(), mem_fn(&Foo::reset));

			auto first = v.data();
			auto last = first + v.size();

			timer<processor_clock> w;
			auto result = scenario.second(first, last);
			m += w.elapsed<microseconds>();
		}
		cout << "  " << scenario.first << " took an average of " << m.avg().count() << " us." << endl;
	}
}

TESTFIXTURE(get_lowest)