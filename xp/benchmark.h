#ifndef __BENCHMARK_H__
#define __BENCHMARK_H__

#include <algorithm>
#include <cassert>
#include <chrono>
#include <ratio>
#include <tuple>

#include "fakeconcepts.h"

namespace xp {

	template<typename C>
	requires(C is Chrono)
	struct stopwatch {
		typedef std::chrono::time_point<C> time_point;
		typedef typename C::duration duration;

		const time_point since;

		stopwatch() : since(C::now()) {}

		template<typename D>
		requires(D is Duration)
			D elapsed() const {
				return std::chrono::duration_cast<D>(C::now() - since);
			}
	};

	template<typename D>
	requires(D is Duration)
	class mesures {
		D   mini;
		D   maxi;
		D   sum;
		int n;

	public:
		mesures() : n(0), mini(D::min()), maxi(D::max()), sum(D::zero()) {}

		mesures& operator += (D d) {
			std::tie(mini, maxi) = std::minmax({mini, d, maxi});
			sum += d;
			n++;
			return *this;
		}
		D min() const { assert(n > 0); return mini; }
		D max() const { assert(n > 0); return maxi; }
		D avg() const { assert(n > 0); return sum / n; }
	};

} // namespace xp

#endif // __BENCHMARK_H__