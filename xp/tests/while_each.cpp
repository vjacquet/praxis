#include <cassert>

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <utility>
#include <vector>

#include "../fakeconcepts.h"
#include "../benchmark.h"
#include "testbench.h"

using namespace std;
using namespace std::chrono;
using namespace xp;

namespace {

	struct true_predicate {
		template<typename T>
		bool operator()(const T&) const {
			return true;
		}

		template<typename T>
		bool operator()(const T&, const T&) const {
			return true;
		}
	};

	template <InputIterator I, Predicate Pred>
	typename iterator_traits<I>::difference_type count_while(I first, I last, Pred pred) {
		typename iterator_traits<I>::difference_type ret = 0;
		while (first != last && pred(*first)) {
			++ret;
			++first;
		}
		return ret;
	}

	template<InputIterator I, Predicate Pred>
	struct take_while_adapter {
		struct iterator_t : public iterator<input_iterator_tag, typename iterator_traits<I>::value_type> {
			take_while_adapter& adapter;
			I it;

			iterator_t(take_while_adapter& adapter, I it) : it(it), adapter(adapter) {}

			iterator_t& operator++() {
				++it;
				if (it != adapter.last.it && !adapter.pred(*it)) {
					it = adapter.last.it;
				}
				return *this;
			}
			iterator_t& operator++(int) {
				iterator_t tmp(*this);
				operator++();
				return tmp;
			}
			bool operator==(const iterator_t& rhs) { return it == rhs.it; }
			bool operator!=(const iterator_t& rhs) { return it != rhs.it; }
			value_type& operator*() {
				return *it;
			}
		};
		Pred pred;
		I first;
		iterator_t last;

		take_while_adapter(I f, I l, Pred p) : pred(p), first(f), last {*this, l} {}

		iterator_t begin() {
			return iterator_t {*this, first};
		}
		iterator_t end() {
			return last;
		}
	};

	template<InputIterator I, Predicate Pred>
	take_while_adapter<I, Pred> take_while(I first, I last, Pred pred) {
		return take_while_adapter<I, Pred>(first, last, pred);
	}

	template<typename T>
	struct counter {
		size_t count;
		const T& value;

		counter(const T& v) : count(0), value(v);
		void operator()(const T& x) {
			if (value == x)
				++count;
		}

		operator size_t() const { return count; }
	};

	template<typename T, Predicate Pred>
	struct counter_if {
		size_t count;
		Pred pred;

		counter_if() : count(0), pred() {}
		counter_if(Pred pred) : count(0), pred(pred) {}
		void operator()(const T& x) {
			if (pred(x))
				++count;
		}

		operator size_t() const { return count; }
	};

	template<typename T, Predicate Pred>
	struct counting_t {
		size_t count;
		Pred& pred;

		counting_t(Pred& p) : count(0), pred(p) {}
		bool operator ()(const T& x) {
			if (!pred(x))
				return false;
			++count;
			return true;
		}
	};

	template<InputIterator I, Predicate Pred, Function Func>
	pair<Func, I> while_each(I first, I last, Pred pred, Func fn) {
		while (first != last && pred(*first)) {
			fn(*first);
			++first;
		}
		return make_pair(move(fn), first);
	}
}

struct predicate_t {
	int stop;
	int count;

	predicate_t(int stop) : stop(stop), count(0) {}
	predicate_t(const predicate_t& other) = default;
	predicate_t& operator=(const predicate_t& other) = default;

	bool operator()(int v) {
		return count++ != stop;
	}
};
typedef size_t(*scenario_t)(vector<int>& v, predicate_t pred);

size_t scenario1(vector<int>& v, predicate_t pred) {
	// specilized algorithm
	return count_while(begin(v), end(v), pred);
}

size_t scenario2(vector<int>& v, predicate_t pred) {
	// wierd and inefficient. The iterator jump to the end once the predicate is false.
	auto adapter = take_while(begin(v), end(v), pred);
	return distance(begin(adapter), end(adapter)) - 1;
}

size_t scenario3(vector<int>& v, predicate_t pred) {
	// standard algorithm with a special predicate. 
	// Usable but not easy to understand.
	counting_t<int, predicate_t> counter(pred);
	auto it = find_if_not(begin(v), end(v), ref(counter));
	return counter.count;
}

size_t scenario4(vector<int>& v, predicate_t pred) {
	// custom algorithm with lambda predicate
	size_t count = 0;
	while_each(begin(v), end(v), pred, [&](int i) { ++count; });
	return count;
}

size_t scenario5(vector<int>& v, predicate_t pred) {
	// custom algorithm with counter
	counter_if<int, true_predicate> counter;
	return while_each(begin(v), end(v), pred, counter).first;
}

TESTBENCH()

TEST(bench_count_while_scenarii) {
	const int attempts = 20000;
	const uniform_int_distribution<int> distribution(400000, 500000);

	vector<int> v;
	v.reserve(500000);
	generate_n(back_inserter(v), 500000, bind(distribution, mt19937 {1664}));

	vector<pair<string, scenario_t>> scenarii {
		make_pair("count_while", &scenario1),
		make_pair("take_while and distance", &scenario2),
		make_pair("find_if_not and counter predicate", &scenario3),
		make_pair("while_each and lambda", &scenario4),
		make_pair("while_each and predicate", &scenario5),
	};

	for (auto& scenario : scenarii) {
		mt19937 rand(1789);
		auto generator = bind(distribution, rand);
		measures<microseconds> m;
		for (int attempt = 0; attempt != attempts; ++attempt) {
			auto stop = generator();
			predicate_t pred {stop};

			timer<high_resolution_clock> w;
			if (stop != scenario.second(v, pred))
				throw runtime_error("Invalid algorithm");

			m += w.elapsed<microseconds>();
		}
		cout << "  " << scenario.first << " took an average of " << m.avg().count() << " us." << endl;
	}
}

TESTFIXTURE(while_each)

