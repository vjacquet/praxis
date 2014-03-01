#ifndef __NUMERIC_H__
#define __NUMERIC_H__

#include <algorithm>
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

	template <OutputIterator O, typename N, typename T>
	O iota_n(O first, N n, T val)
	{
		for (; 0 < n; --n, ++val)
			*first = val;
		return first;

		// The version above is faster. 
		// Too bad, I'd rather used existing algorithms when possible.
		//auto gen = details::iota_generator<T>{val};
		//return generate_n(first, n, gen);
	}

} // namespace xp

#endif __NUMERIC_H__