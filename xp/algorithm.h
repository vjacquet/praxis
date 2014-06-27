#ifndef __ALGORITHM_H__
#define __ALGORITHM_H__

#include <algorithm>
#include <iterator>
#include <utility>

#include "fakeconcepts.h"

namespace xp {

	namespace details {

		template<BidirectionalIterator I, class T>
		I find_backwards_with_not_found_value(I first, I last, I not_found, const T& val) {
			auto it = last;
			while(it != first) {
				--it;
				if(*it == val) return it;
			}
			return not_found;
		}

		template<BidirectionalIterator I, UnaryPredicate Pred>
		I find_if_backwards_with_not_found_value(I first, I last, I not_found, Pred pred) {
			auto it = last;
			while(it != first) {
				--it;
				if(pred(*it)) return it;
			}
			return not_found;
		}

		// The cost function returns a value supporting LessThanComparable
		template<ForwardIterator I, Function Cost>
		inline
		I min_cost_element_nonempty(I first, I last, Cost cost) {
			typedef decltype(cost(*first)) C;
			C lowest = cost(*first);
			auto found = first;
			while (++first != last) {
				auto val = cost(*first);
				if (val < lowest) {
					lowest = val;
					found = first;
				}
			}
			return found;
		}

		// The cost function returns a value supporting LessThanComparable
		template<ForwardIterator I, Function Cost>
		I max_cost_element_nonempty(I first, I last, Cost cost) {
			auto highest = cost(*first);
			auto found = first;
			for (++first; first != last; ++first) {
				auto val = cost(*first);
				if (highest < val) {
					highest = val;
					found = first;
				}
			}
			return found;
		}

	}

template<BidirectionalIterator I, typename T>
I find_backwards(I first, I last, const T& val) {
	return details::find_backwards_with_not_found_value(first, last, last, val);
}

template<BidirectionalIterator I, UnaryPredicate Pred>
I find_if_backwards(I first, I last, Pred pred) {
	return details::find_if_backwards_with_not_found_value(first, last, last, pred);
}

template<BidirectionalIterator I, typename T>
requires(first <= hint && hint < last)
I find(I first, I last, I hint, const T& val) {
	I lo = hint;
	I hi = hint + 1;
	for(;;) {
		if(*lo == val) return lo;
		if(hi == last) return details::find_backwards_with_not_found_value(first, lo, last, val);
		if(*hi == val) return hi;
		++hi;
		if(lo == first) return find(hi, last, val);
		--lo;
	}
}

// because of cache misses, it might not be best if the hint is too far from the target
template<BidirectionalIterator I, UnaryPredicate Pred>
requires(first <= hint && hint < last)
I find_if(I first, I last, I hint, Pred pred) {
	I lo = hint;
	I hi = hint + 1;
	for(;;) {
		if(pred(*lo)) return lo;
		if(hi == last) return details::find_if_backwards_with_not_found_value(first, lo, last, pred);
		if(pred(*hi)) return hi;
		++hi;
		if(lo == first) return find_if(hi, last, pred);
		--lo;
	}
}

template <class T, StrictWeakOrdering Compare>
bool is_sorted(const T& a, const T& b, Compare cmp) {
	return !cmp(b, a);
}

template<typename T>
bool is_sorted(const T& a, const T& b) {
	return is_sorted(a, b, std::less<T>());
}

template <class T, StrictWeakOrdering Compare>
bool is_sorted(const T& a, const T& b, const T& c, Compare cmp) {
	return !(cmp(b, a) || cmp(c, b));
}

template<typename T>
bool is_sorted(const T& a, const T& b, const T& c) {
	return is_sorted(a, b, c, std::less<T>());
}

// different from boost' clamp function because the value can be of a different type than min & max.
template<typename T, typename U>
T tighten(U val, T min, T max) {
	if (val > static_cast<U>(max))
		return max;
	else if (val < static_cast<U>(min))
		return min;
	return static_cast<T>(val);
}

template<typename T>
std::pair<const T&, const T&> adjust_minmax(const T& cmin, const T& cmax, const T& c) {
	// precondition: cmin < cmax
	if (c < cmin) return {c, cmax};
	if (cmax < c) return {cmin, c};
	return {cmin, cmax};
}

template<typename T, StrictWeakOrdering Compare>
std::pair<const T&, const T&> adjust_minmax(const T& cmin, const T& cmax, const T& c, Compare cmp) {
	// precondition: cmp(cmin, cmax)
	if (cmp(c, cmin)) return {c, cmax};
	if (cmp(cmax, c)) return {cmin, c};
	return {cmin, cmax};
}

template <ForwardIterator I>
std::pair<DifferenceType(I), I> count_while_adjacent(I first, I last)
{
	DifferenceType(I) n {0};
	if (first != last) {
		I next = first;
		++n;
		++next;
		while (next != last) {
			if (*first != *next)
				return {n, next};
			++n;
			++next;
		}
	}
	return {n, last};
}

template<ForwardIterator I, OutputIterator O>
O unique_copy_with_count(I first, I last, O output) {
	I next;
	DifferenceType(I) n;
	while (first != last) {
		tie(n, next) = count_while_adjacent(first, last);
		*output = {n, *first};
		++output;
		first = next;
	}
	return output;
}

template<typename T, StrictWeakOrdering Compare>
const T& stable_max(const T& a, const T& b, Compare cmp) {
	// Returns the second argument when the arguments are equivalent.
	if (cmp(b, a))
		return a;
	return b;
}

template<typename T>
const T& stable_max(const T& a, const T& b) {
	// Returns the second argument when the arguments are equivalent.
	return stable_max(a, b, std::less<T>());
}

template <ForwardIterator I, StrictWeakOrdering Compare>
I stable_max_element(I first, I last, Compare cmp) {
	if (first == last) return last;
	I largest = first;
	while (++first != last) {
		if (!cmp(*first, *largest))
			largest = first;
	}
	return largest;
}

template<ForwardIterator I>
I stable_max_element(I first, I last) {
	return stable_max_element(first, last, std::less<T>());
}


// The cost function returns a value supporting LessThanComparable
template<ForwardIterator I, Function Cost>
inline
I min_cost_element(I first, I last, Cost cost) {
	if (first == last) return last;
	return details::min_cost_element_nonempty(first, last, cost);
}

// The cost function returns a value supporting LessThanComparable
template<ForwardIterator I, Function Cost>
I max_cost_element(I first, I last, Cost cost) {
	if (first == last) return last;
	return details::max_cost_element_nonempty(first, last, cost);
}

// The cost function returns a value supporting LessThanComparable
template<ForwardIterator I, Function Cost>
std::pair<I, I> minmax_cost_element(I first, I last, Cost cost) {
	using namespace std;

	auto result = make_pair(first, first);
	if (first != last) {
		auto lowest = cost(*first);
		auto highest = lowest;
		for (++first; first != last; ++first) {
			auto val = cost(*first);
			if (val < lowest) {
				lowest = val;
				result.first = first;
			} else if (!(val < highest)) {
				highest = val;
				result.second = first;
			}
		}
	}
	return result;
}

template<Range R>
R range_before(R const& range, decltype(*std::cbegin(range)) const& val) {
	auto first = std::cbegin(range);
	auto last = std::cend(range);
	auto found = std::find(first, last, val);
	if (found == last)
		return {last, last};
	return {first, found};
}

template<Range R>
R range_after(R const& range, decltype(*std::cbegin(range)) const& val) {
	auto first = std::cbegin(range);
	auto last = std::cend(range);
	auto found = std::find(first, last, val);
	if (found != last)
		++found;
	return {found, last};
}

template<Range R1, Range R2>
R1 range_before(R1 const& range1, R2 const& range2) {
	auto first1 = std::cbegin(range1);
	auto last1 = std::cend(range1);
	auto found = std::search(first1, last1, std::cbegin(range2), std::cend(range2));
	if (found == last1)
		return {last1, last1};
	return {first1, found};
}

template<Range R1, Range R2>
R1 range_after(R1 const& range1, R2 const& range2) {
	auto first1 = std::cbegin(range1);
	auto last1 = std::cend(range1);
	auto found = std::search(first1, last1, std::cbegin(range2), std::cend(range2));
	if (found != last1)
		++found;
	return {found, last1};
}

template<ForwardIterator I>
struct bounded_range {
	I first;
	I last;

	inline friend bool operator == (const bounded_range& x, const bounded_range& y) {
		return x.first == y.first && x.last == y.last;
	}
	inline friend bool operator != (const bounded_range& x, const bounded_range& y) {
		return !(x == y);
	}
};

template<ForwardIterator I>
bounded_range<I> make_bounded_range(I first, I last) {
	return bounded_range<I> {first, last};
}

template<ForwardIterator I>
struct counted_range {
	I first;
	typename std::iterator_traits<I>::difference_type n;

	inline friend bool operator == (const counted_range& x, const counted_range& y) {
		return x.first == y.first && x.n == y.n;
	}
	inline friend bool operator != (const counted_range& x, const counted_range& y) {
		return !(x == y);
	}
};

template<ForwardIterator I, Number N>
counted_range<I> make_counted_range(I first, N n) {
	return counted_range<I> {first, std::iterator_traits<I>::difference_type(n)};
}

} // namespace xp

#endif __ALGORITHM_H__
