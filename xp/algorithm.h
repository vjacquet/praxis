#ifndef __ALGORITHM_H__
#define __ALGORITHM_H__

#include <algorithm>
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

	}

	template<BidirectionalIterator I, typename T>
I find_backwards(I first, I last, const T& val) {
	return details::find_backwards_with_not_found_value(first, last, last, val);
}

template<BidirectionalIterator I, UnaryPredicate Pred>
I find_if_backwards(I first, I last, Pred pred) {
	return details::find_if_backwards_with_not_found_value(first, last, last, val);
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

// different from boost' clamp function because the value can be of a different type than min & max.
template<typename T, typename U>
T tighten(U val, T min, T max) {
	if (val > static_cast<U>(max))
		return max;
	else if (val < static_cast<U>(min))
		return min;
	return static_cast<T>(val);
}

} // namespace xp

#endif __ALGORITHM_H__
