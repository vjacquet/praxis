#ifndef __NUMERIC_H__
#define __NUMERIC_H__

#include <algorithm>
#include <chrono>
#include <random>
#include "fakeconcepts.h"

namespace xp {

	namespace details {
		template<typename T>
		struct iota_generator {
			T val;
			iota_generator(T v) : val(v){}
			T operator()() {
				return val++;
			}
		};
	} // namespace details

	template<OutputIterator O, typename N, typename T>
	O iota_n(O first, N n, T val)
	{
		for (; 0 < n; --n, ++first, ++val)
			*first = val;
		return first;

		// The version above is faster by almost 20%. 
		// That's unfortunate, I'd rather use existing algorithms whenever possible.
		//auto gen = details::iota_generator<T>{val};
		//return generate_n(first, n, gen);
	}

	template<RandomAccessIterator I, typename T, class URNG>
	I random_iota(I first, I last, T val, URNG&& g) {
		std::iota(first, last, val);
		std::shuffle(first, last, std::forward<URNG>(g));
		return last;
	}

	template<RandomAccessIterator I, typename T>
	I random_iota(I first, I last, T val) {
		auto seed = std::random_device();
		return random_iota(first, last, val, std::default_random_engine {seed()});
	}

} // namespace xp

#endif __NUMERIC_H__