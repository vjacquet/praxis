#include <cstddef>
#include <exception>
#include <functional>
#include <initializer_list>
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

	instrumented& operator=(const instrumented&) = delete;
};

struct is_not_end_of_string {
	bool operator()(const char * c) {
		return *c != '\0';
	}
};

template<typename T>
struct first_less {
	bool operator()(pair<T, T> x, pair<T, T> y) {
		return x.first < y.first;
	}
};

struct counting_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
	std::ptrdiff_t count;

	counting_iterator() : count() {}
	counting_iterator(counting_iterator const& x) : count(x.count) {}

	template<typename T>
	counting_iterator& operator= (T const &) {
		++count;
		return *this;
	}
	counting_iterator& operator* () {
		return *this;
	}
	counting_iterator& operator++ () {
		return *this;
	}
	counting_iterator& operator++ (int) {
		return *this;
	}
};

template<class F, class...Ts>
F for_each_arg(F f, Ts&&...a) {
	return std::initializer_list<int>{(f(a), 0)...}, f;
}

struct dump {
	template<typename T>
	void operator()(T const& t) {
		std::cout << t << endl;
	}
};

TESTBENCH()

TEST(check_for_each_arg) {
	for_each_arg(dump {}, "a", 'b', 3);
}

TEST(check_unique_count_with_adapter) {
	vector<int> v {0, 11, 33, 33, 44, 66, 66, 77, 88, 99};
	auto counter = std::unique_copy(v.begin(), v.end(), counting_iterator {});
	VERIFY_EQ(8, counter.count);
}

TEST(check_unique_count) {
	vector<int> v {0, 11, 33, 33, 44, 66, 66, 77, 88, 99};
	auto counter = xp::unique_count(v.begin(), v.end());
	VERIFY_EQ(8, counter);
}

TEST(check_find_backwards) {
	vector<int> v{0, 11, 22, 33, 44, 55, 66, 77, 88, 99};

	auto found = find_backward(v.cbegin(), v.cend(), 44);
	auto d = distance(v.cbegin(), found);
	VERIFY(d == 4);

	auto not_found = find_backward(v.cbegin(), v.cend(), 60);
	VERIFY(not_found == v.cend());
}

TEST(check_find_if_not_backwards) {
	vector<int> v {0, 11, 22, 33, 44, 55, 66, 77, 88, 99};

	auto found = find_if_not_backward(v.cbegin(), v.cend(), [](int x) { return x != 44; });
	auto d = distance(v.cbegin(), found);
	VERIFY(d == 4);

	auto not_found = find_if_not_backward(v.cbegin(), v.cend(), [](int x) { return x != 60; });
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

TEST(check_min_cost_element) {
	vector<int> v { 2, 5, 7, 8, 2, 42, 8, 42, 1, 23, 1 };
	auto result = min_cost_element(v.begin(), v.end(), [](int i) { return i * i; });

	VERIFY(distance(v.begin(), result) == 8);
}

TEST(check_max_cost_element) {
	vector<int> v {2, 5, 7, 8, 2, 42, 8, 42, 1, 23, 1};
	auto result = max_cost_element(v.begin(), v.end(), [](int i) { return i * i; });

	VERIFY(distance(v.begin(), result) == 5);
}

TEST(check_minmax_cost_element) {
	vector<int> v {2, 5, 7, 8, 2, 42, 8, 42, 1, 23, 1};
	auto result = minmax_cost_element(v.begin(), v.end(), [](int i) { return i * i; });

	VERIFY(distance(v.begin(), result.first) == 8);
	// Different from max_cost_element but consistent with minmax_element
	VERIFY(distance(v.begin(), result.second) == 7); 
}

TEST(check_count_while_adjacent) {
	vector<int> v {1, 1, 1, 2, 2, 3, 4, 4, 4, 4};

	auto f = v.begin();
	auto l = v.end();
	int c = 0;
	tie(c, f) = count_while_adjacent(f, l);
	VERIFY(c == 3);
	tie(c, f) = count_while_adjacent(f, l);
	VERIFY(c == 2);
	tie(c, f) = count_while_adjacent(f, l);
	VERIFY(c == 1);
	tie(c, f) = count_while_adjacent(f, l);
	VERIFY(c == 4);
}

TEST(check_unique_copy_with_count) {
	vector<int> v {1, 1, 1, 2, 2, 3, 4, 4, 4, 4};
	vector<pair<int, int>> expected {{3, 1}, {2, 2}, {1, 3}, {4, 4}};
	vector<pair<int, int>> actual;
	unique_copy_with_count(v.begin(), v.end(), back_inserter(actual));
	VERIFY(expected.size() == actual.size());
	VERIFY(equal(actual.begin(), actual.end(), expected.begin()));
}

TEST(check_stable_max) {
	first_less<int> cmp;

	pair<int, int> x {0, 1};
	pair<int, int> y {0, 2};
	VERIFY(std::max(x, y, cmp).second == 1);
	VERIFY(stable_max(x, y, cmp).second == 2);
}

TEST(check_stable_max_element) {
	first_less<int> cmp;

	vector<pair<int, int>> v {{3, 1}, {2, 2}, {1, 3}, {3, 4}};

	VERIFY(std::max_element(v.begin(), v.end(), cmp)->second == 1);
	VERIFY(stable_max_element(v.begin(), v.end(), cmp)->second == 4);
}

TEST(check_copy_while) {
	vector<char> v;
	copy_while("hello world!", is_not_end_of_string {}, back_inserter(v));

	VERIFY(v.size() == 12);
	VERIFY(v.front() == 'h');
	VERIFY(v.back() == '!');
}

TEST(check_fill_while) {
	char s[] = "hello world!";
	fill_while(s, is_not_end_of_string {}, '_');
	VERIFY(*s == '_');
	VERIFY(count(s, s + 12, '_') == 12);
}

TEST(check_find_while) {
	auto s = "hello world!";
	auto w = find_while(s, is_not_end_of_string {}, ' ');
	VERIFY(distance(s, w) == 5);
}

TESTFIXTURE(algorithm)
