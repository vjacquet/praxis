#include <iostream>
#include <utility>
#include <vector>

using std::vector;

#include "../fakeconcepts.h"

#include "testbench.h"

struct dump {
	template<typename I>
	void operator()(I first, I last) {
		std::cout << " [";
		std::copy(first, last, std::ostream_iterator<decltype(*first)>{ std::cout, ", " });
		std::cout << ']';
	}
};

bool is_odd(int i) {
	return (i & 1) == 1;
}

bool is_even(int i) {
	return (i & 1) == 0;
}

template<ForwardIterator I, Searcher S>
auto search(I first, I last, S&& s) -> decltype(s(first, last)) {
	return s(first, last);
}

template<typename T>
struct find_searcher {
	T val;

	find_searcher(const T& val) : val(val) {
	}

	template<ForwardIterator I>
	std::pair<I, I> operator()(I first, I last) const {
		first = std::find(first, last, val);
		if (first != last)
			last = first + 1;
		return{ first, last };
	}
};

template<typename Pred>
struct find_if_searcher {
	Pred pred;

	find_if_searcher(Pred&& pred) : pred(std::forward<Pred>(pred)) {
	}

	template<ForwardIterator I>
	std::pair<I, I> operator()(I first, I last) const {
		first = std::find_if(first, last, pred);
		if (first != last)
			last = first + 1;
		return{ first, last };
	}
};

template<class Pred>
find_if_searcher<Pred> make_find_if_searcher(Pred&& pred) {
	return find_if_searcher<Pred>{std::forward<Pred>(pred)};
}

template<ForwardIterator I, typename C = std::iterator_traits<I>::iterator_category>
I cbegin(const std::pair<I, I>& r) {
	return r.first;
}

template<ForwardIterator I, typename C = std::iterator_traits<I>::iterator_category>
I cend(const std::pair<I, I>& r) {
	return r.second;
}

template<ForwardIterator I, Searcher S, BinaryFunction F>
F split(I first, I last, S& s, F f) {
	while (first != last) {
		auto found = s(first, last);
		auto begin = cbegin(found);
		auto end = cend(found);
		f(first, begin);
		if (end == last && begin != end)
			f(last, last);
		first = end;
	}
	return f;
}

TESTBENCH()

TEST(check_split) {
	vector<int> v{ 0, 11, 33, 33, 44, 66, 66, 77, 88, 99, 121 };

	auto searcher = make_find_if_searcher(is_even);
	split(begin(v), end(v), searcher, dump{});
}

TEST(check_search) {
	vector<int> v{ 0, 11, 33, 33, 44, 66, 66, 77, 88, 99 };

	find_searcher<int> searcher(44);
	auto found = search(cbegin(v), cend(v), searcher);
	VERIFY_EQ(4, std::distance(cbegin(v), found.first));
	VERIFY_EQ(1, std::distance(found.first, found.second));
	VERIFY_EQ(5, std::distance(found.second, cend(v)));
}

TESTFIXTURE(split)
