#ifndef __ALGORITHM_H__
#define __ALGORITHM_H__

#include <algorithm>
#include <functional>
#include <iterator>
#include <stack>
#include <utility>

#include "fakeconcepts.h"
#include "functional.h"

namespace xp {

	namespace details {

		template<BidirectionalIterator I, class T>
		I find_backward_with_not_found_value(I first, I last, I not_found, const T& val) {
			auto it = last;
			while(it != first) {
				--it;
				if(*it == val) return it;
			}
			return not_found;
		}

		template<BidirectionalIterator I, Predicate Pred>
		I find_if_backward_with_not_found_value(I first, I last, I not_found, Pred pred) {
			auto it = last;
			while(it != first) {
				--it;
				if(pred(*it)) return it;
			}
			return not_found;
		}

		// The cost function returns a value supporting LessThanComparable
		template<ForwardIterator I, Function Cost, Relation Compare>
		inline
		I compare_cost_element_nonempty(I first, I last, Cost cost, Compare cmp) {
			auto lowest = cost(*first);
			auto found = first;
			while (++first != last) {
				auto val = cost(*first);
				if (cmp(val, lowest)) {
					lowest = val;
					found = first;
				}
			}
			return found;
		}
		template<Relation Compare>
		struct transpose {
			template<typename T, typename U>
			bool operator()(T x, U y) {
				return Compare()(y, x);
			}
		};
		template<Relation Compare>
		struct negate {
			template<typename T, typename U>
			bool operator()(T x, U y) {
				return !Compare()(x, y);
			}
		};

	}

template<BidirectionalIterator I, typename T>
I find_backward(I first, I last, const T& val) {
	return details::find_backward_with_not_found_value(first, last, last, val);
}

template<BidirectionalIterator I, Predicate Pred>
I find_if_backward(I first, I last, Pred pred) {
	return details::find_if_backward_with_not_found_value(first, last, last, pred);
}

template<BidirectionalIterator I, Predicate Pred>
I find_if_not_backward(I first, I last, Pred pred) {
	return details::find_if_backward_with_not_found_value(first, last, last, negation(pred));
}

template<BidirectionalIterator I, typename T>
requires(first <= hint && hint < last)
I find(I first, I last, I hint, const T& val) {
	I lo = hint;
	I hi = hint + 1;
	for(;;) {
		if(*lo == val) return lo;
		if(hi == last) return details::find_backward_with_not_found_value(first, lo, last, val);
		if(*hi == val) return hi;
		++hi;
		if(lo == first) return find(hi, last, val);
		--lo;
	}
}

// because of cache misses, it might not be best if the hint is too far from the target
template<BidirectionalIterator I, Predicate Pred>
requires(first <= hint && hint < last)
I find_if(I first, I last, I hint, Pred pred) {
	I lo = hint;
	I hi = hint + 1;
	for(;;) {
		if(pred(*lo)) return lo;
		if(hi == last) return details::find_if_backward_with_not_found_value(first, lo, last, pred);
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

namespace details {

	template<InputIterator I, Integer N, OutputIterator O>
	std::pair<I, O> copy_atmost_n(I first, I last, N n, O output, std::input_iterator_tag) {
		while (first != last && n != 0) {
			*output = *first;
			++first;
			--n;
			++output;
		}
		return {first, output};
	}

	template<RandomAccessIterator I, Integer N, OutputIterator O>
	std::pair<I, O> copy_atmost_n(I first, I last, N n, O output, std::random_access_iterator_tag) {
		last = first + std::min(std::iterator_traits<I>::difference_type (n), std::distance(first, last));
		return {last, std::copy(first, last, output)};
	}

}

template<InputIterator I, Integer N, OutputIterator O>
std::pair<I, O> copy_atmost_n(I first, I last, N n, O output) {
	return details::copy_atmost_n(first, last, n, output, std::iterator_traits<I>::iterator_category());
}

template<InputIterator I, Function F>
void for_each_element(I f, I l, F fn) {
	while (f != l) {
		fn(f);
		++f;
	}
	return std::move(fn);
}

// stable_max is an attempt to render coherent the fact that std::max is unstable.
// because stability migth have a small performance cost, as it assign the max for all
// equivalent values, instead of the just the first one.
// Nevertheless, the argument does not hold because of minmax, that is stable by definition 
// for two arguments, and that has been specified as such for more than two.
// So, having max returning something different from the max of minmax is clearly a bug
// and not just "too theorical".
// The naming convention should be:
// - do not prefix when the stability is expected or when it is not required
// - do prefix with stable when the stability is a secondary goal that can be reach with an
//   additional cost
// - do prefix with unstable when the stability s expected but removing it makes it 
//   more efficient
// Therefore, this method is unfortunatelly badly named.
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
	return details::compare_cost_element_nonempty(first, last, cost, std::less<>());
}

// The cost function returns a value supporting LessThanComparable
template<ForwardIterator I, Function Cost>
I max_cost_element(I first, I last, Cost cost) {
	if (first == last) return last;
	return details::compare_cost_element_nonempty(first, last, cost, details::transpose<std::less<>>());
}

// The cost function returns a value supporting LessThanComparable
template<ForwardIterator I, Function Cost>
I stable_max_cost_element(I first, I last, Cost cost) {
	if (first == last) return last;
	return details::compare_cost_element_nonempty(first, last, cost, details::negate<std::less<>>());
}

// The cost function returns a value supporting LessThanComparable
template<ForwardIterator I, Function Cost>
std::pair<I, I> minmax_cost_element(I first, I last, Cost cost) {
	using namespace std;

	if (first == last) {
		return {last, last};

	auto min = first;
	auto max = first;
	auto lowest = cost(*first);
	auto highest = lowest;
	for (++first; first != last; ++first) {
		auto val = cost(*first);
		if (val < lowest) {
			lowest = val;
			min = first;
		} else if (!(val < highest)) {
			highest = val;
			max = first;
		}
	}
	return {min, max};
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
	typedef typename I iterator;

	I first;
	I last;

	bounded_range() : first(), last(first) {}
	bounded_range(I first, I last) : first(first), last(last) {}

	inline friend bool operator == (const bounded_range& x, const bounded_range& y) {
		return x.first == y.first && x.last == y.last;
	}
	inline friend bool operator != (const bounded_range& x, const bounded_range& y) {
		return !(x == y);
	}

	I begin() const { return first; }
	I end() const { return last; }
};

template<ForwardIterator I>
bounded_range<I> make_bounded_range(I first, I last) {
	return bounded_range<I> {first, last};
}

template<ForwardIterator I>
struct counted_range {
	typedef typename I iterator;
	typedef typename std::iterator_traits<I>::difference_type size_type;

	I first;
	size_type n;

	counted_range() : first(), n(size_type(0)) {}
	counted_range(I first, size_type n) : first(first), n(n) {}

	inline friend bool operator == (const counted_range& x, const counted_range& y) {
		return x.first == y.first && x.n == y.n;
	}
	inline friend bool operator != (const counted_range& x, const counted_range& y) {
		return !(x == y);
	}

	I begin() const { return first; }
	size_type size() const { return n; }
};

template<ForwardIterator I, Number N>
counted_range<I> make_counted_range(I first, N n) {
	return counted_range<I> {first, std::iterator_traits<I>::difference_type(n)};
}

template<typename T>
inline auto size(const T& x) -> decltype(x.size()) {
	return x.size();
}

template<typename T, std::size_t N>
inline std::size_t size(T(&)[N]) {
	return N;
}

template<Range R>
inline auto size(const R& range) -> typename std::iterator_traits<decltype(std::cend(range))>::difference_type {
	return std::distance(std::cbegin(range), std::cend(range));
}

template<typename T>
inline bool empty(const T& x) {
	return x.empty();
}

template<ForwardIterator I>
inline bool empty(const bounded_range<I>& x) {
	return std::cbegin(x) == std::cend(x);
}

template<ForwardIterator I>
inline bool empty(const counted_range<I>& x) {
	return x.size() == 0;
}

template<InputIterator I, Predicate Guard>
struct guarded_range {
	typedef typename I iterator;
	typedef typename Guard predicate;

	iterator first;
	predicate guard;

	guarded_range() : first(), guard() {}
	guarded_range(I first, Guard guard) : first(first), guard(guard) {}

	inline friend bool operator == (const guarded_range& x, const guarded_range& y) {
		return x.first == y.first && x.guard == y.guard;
	}
	inline friend bool operator != (const guarded_range& x, const guarded_range& y) {
		return !(x == y);
	}

	iterator begin() const { return first; }
};

// The predicate returns true if the iterator is valid.
template<InputIterator I, Predicate Guard, OutputIterator O>
O copy_while(I first, Guard guard, O result) {
	while (guard(first)) {
		*result = *first;
		++first;
		++result;
	}
	return result;
}

template<OutputIterator O, Predicate Guard, class T>
O fill_while(O first, Guard guard, const T& val) {
	while (guard(first)) {
		*first = val;
		++first;
	}
	return first;
}

template<InputIterator I, Predicate Guard, class T>
I find_while(I first, Guard guard, const T& val)
{
	while (guard(first) && *first != val) {
		++first;
	}
	return first;
}

template<InputIterator I, Predicate Guard, Predicate Pred>
I find_if_while(I first, Guard guard, Pred pred)
{
	while (guard(first) && !pred(*first)) {
		++first;
	}
	return first;
}

template<InputIterator I, Predicate Guard, OutputIterator O, UnaryOperation Op>
O transform_while(I first, Guard guard, O result, Op op)
{
	while (guard(first)) {
		*result = op(*first);
		++first;
		++result;
	}
	return result;
}

// both iterators are guarded
template<InputIterator I1, Predicate Guard1, InputIterator I2, Predicate Guard2, OutputIterator O, BinaryOperation Op>
O transform_while(I1 first1, Guard1 guard1, I2 first2, Guard1 guard2, O result, Op op)
{
	while (guard1(first1) && guard2(first2)) {
		*result = op(*first1, *first2);
		++first1;
		++first2;
		++result;
	}
	return result;
}

template<InputIterator I1, InputIterator I2, Relation Guard, OutputIterator O, BinaryOperation Op>
O transform_while(I1 first1, I2 first2, Guard guard, O result, Op op)
{
	while (guard(first1, first2)) {
		*result = op(*first1, *first2);
		++first1;
		++first2;
		++result;
	}
	return result;
}

template<typename T, StrictWeakOrdering Compare>
const T& min(const T& a, const T& b, const T& c, Compare cmp) {
	return std::min(a, std::min(b, c, cmp), cmp);
}

template<typename T>
const T& min(const T& a, const T& b, const T& c) {
	return min(a, b, c, std::less<T>());
}

template<typename T, StrictWeakOrdering Compare>
const T& max(const T& a, const T& b, const T& c, Compare cmp) {
	return stable_max(a, stable_max(b, c, cmp), cmp);
}

template<typename T>
const T& max(const T& a, const T& b, const T& c) {
	return max(a, b, c, std::less<T>());
}

// reduce algorithms are from EoP
template<InputIterator I, BinaryOperation Op, Function F>
auto reduce_nonempty(I first, I last, Op op, F fun) -> decltype(op(*first, *first)) {
	typedef decltype(op(*first, *first)) T;
	T r = fn(first);
	++first;
	while (first != last) {
		r = op(r, fn(first));
		++first;
	}
	return r;
}

template<InputIterator I, BinaryOperation Op>
auto reduce_nonempty(I first, I last, Op op) -> decltype(op(*first, *first)) {
	typedef decltype(op(*first, *first)) T;
	T r = *first;
	++first;
	while (first != last) {
		r = op(r, *first);
		++first;
	}
	return r;
}

template<InputIterator I, typename T, BinaryOperation Op, Function F>
T reduce(I first, I last, Op op, F fun, const T& z) {
	if (first == last) return z;
	return reduce_nonempty(first, last, op, fun);
}

template<InputIterator I, typename T, BinaryOperation Op>
T reduce(I first, I last, Op op, const T& z) {
	if (first == last) return z;
	return reduce_nonempty(first, last, op);
}

template<InputIterator I, typename T, BinaryOperation Op, Function F>
T reduce_nonzeroes(I first, I last, Op op, F fun, const T& z) {
	T x;
	do {
		if (first == last) return z;
		x = fun(first);
		++first;
	} while (x == z);
	while (first != last) {
		T y = fun(first);
		if (y != z) x = op(x, y);
		++first;
	}
	return x;
}

template<InputIterator I, typename T, BinaryOperation Op>
T reduce_nonzeroes(I first, I last, Op op, const T& z) {
	T x;
	do {
		if (first == last) return z;
		x = *first;
		++first;
	} while (x == z);
	while (first != last) {
		T y = *first;
		if (y != z) x = op(x, y);
		++first;
	}
	return x;
}

template<InputIterator I, typename T, BinaryOperation Op>
T foldl(I first, I last, Op op, const T& z) {
	if (first == last) return z;
	return reduce_nonempty(first, last, op);
}

namespace details {

	template<InputIterator I, typename T, BinaryOperation Op>
	T foldr_nonempty(I first, I last, Op op, std::input_iterator_tag) {
		std::stack<std::iterator_traits<I>::value_type> stk;
		std::copy(first, last, std::back_inserter(stk));
		auto r = stk.top();
		stk.pop();
		while (!stk.empty()) {
			r = op(stk.top(), r);
			stk.pop();
		}
		return r;
	}

	template<BidirectionalIterator I, typename T, BinaryOperation Op>
	T foldr_nonempty(I first, I last, Op op, std::bidirectional_iterator_tag) {
		return reduce_nonempty(std::reverse_iterator<I>(last), std::reverse_iterator<I>(first), op);
	}

}
template<InputIterator I, typename T, BinaryOperation Op>
T foldr(I first, I last, Op op, const T& z) {
	if (first == last) return z;
	return details::foldr_nonempty(first, last, op, std::iterator_traits<I>::iterator_category {});
}

template<BidirectionalIterator I, OutputIterator O, UnaryOperation Op>
O transform_backward(I first, I last, O result, Op op) {
	while (first != last)
		*--result = op(*--last);
	return result;
}

template<BidirectionalIterator I1, BidirectionalIterator I2, OutputIterator O, UnaryOperation Op>
O transform_backward(I1 first1, I1 last1, I2 last2, O result, Op op) {
	while (first1 != last1)
		*--result = op(*--last1, *--last2);
	return result;
}

// slide selection to another position.
// ...****......
//          ^
// .....****....
// from sean parent
template<RandomAccessIterator I>
std::pair<I, I> slide(I first, I last, I point) {
	using namespace std;

	if (point < first) return {point, rotate(point, first, last)};
	if (last < point) return {rotate(first, last, point), point};
	return {first, last};
}

// gather selected elements around a position.
// ..**.*....*...
//        ^
// ....****......
// from sean parent
template<BidirectionalIterator I, Predicate P>
std::pair<I, I> gather(I first, I last, I point, P pred) {
	using namespace std;

	return {stable_partition(first, point, not1(pred)), stable_partition(point, last, pred)};
}

template <InputIterator I1, InputIterator I2, Relation Pred>
auto hamming_distance(I1 first1, I1 last1, I2 first2, Pred pred) 
-> typename std::iterator_traits<I1>::difference_type {
	using namespace std;

	typename iterator_traits<I>::difference_type result {};
	while (first1 != last1) {
		if (!pred(*first1, *first2))
			++result;
		++first1;
		++first2;
	}
	return result;
}

template <InputIterator I1, InputIterator I2>
auto hamming_distance(I1 first1, I1 last1, I2 first2) 
-> typename std::iterator_traits<I1>::difference_type {
	using namespace std;

	typedef typename iterator_traits<I>::value_type value_type;
	return hamming_distance(first1, last1, first2, std::equal<value_type>());
}

template <InputIterator I1, InputIterator I2, Integer N, Relation Pred>
N hamming_distance_n(I1 first1, N n, I2 first2, Pred pred) {
	N result = 0;
	while (n--) {
		if (!pred(*first1, *first2))
			++result;
		++first1;
		++first2;
	}
	return result;
}

template <InputIterator I1, InputIterator I2, Integer N>
N hamming_distance_n(I1 first1, N n, I2 first2) {
	using namespace std;

	typedef typename iterator_traits<I>::value_type value_type;
	return hamming_distance_n(first1, last1, first2, std::equal<value_type>());
}

namespace details {

	template <ForwardIterator I, Relation R>
	auto unique_count(I first, I last, R rel, std::input_iterator_tag)
	-> typename std::iterator_traits<I>::difference_type 
	{
		using namespace std;

		typename iterator_traits<I>::difference_type result {};
		if (first == last) return result;

		typename iterator_traits<I>::value_type r = *first;
		while (++first != last) {
			if (!rel(r, *first)) {
				++result;
				r = *first;
			}
		}
		return ++result;
	}

	template <ForwardIterator I, Relation R>
	auto unique_count(I first, I last, R rel, std::forward_iterator_tag)
	-> typename std::iterator_traits<I>::difference_type
	{
		using namespace std;

		typename iterator_traits<I>::difference_type result {};
		if (first == last) return result;

		typename I r = first;
		while (++first != last) {
			if (!rel(*r, *first)) {
				++result;
				r = first;
			}
		}
		return ++result;
	}
}

template <ForwardIterator I, Relation R>
auto unique_count(I first, I last, R rel)
-> typename std::iterator_traits<I>::difference_type
{
	return details::unique_count(first, last, rel, std::iterator_traits<I>::iterator_category());
}

template <ForwardIterator I>
auto unique_count(I first, I last)
-> typename std::iterator_traits<I>::difference_type
{
	using namespace std;

	typedef typename iterator_traits<I>::value_type value_type;
	return details::unique_count(first, last, equal_to<value_type>(), iterator_traits<I>::iterator_category());
}

} // namespace xp

#endif __ALGORITHM_H__
