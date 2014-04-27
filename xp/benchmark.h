#ifndef __BENCHMARK_H__
#define __BENCHMARK_H__

#include <cassert>
#include <ctime>
#include <algorithm>
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

	class processor_clock {
	public:
		typedef clock_t rep;
		typedef std::ratio<1, CLOCKS_PER_SEC> period;
		typedef std::chrono::duration<rep, period> duration;
		typedef std::chrono::time_point<processor_clock> time_point;

		static const bool is_steady = true;

		static time_point now() {
			return time_point {duration {std::clock()}};
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
		mesures() : mini(D::min()), maxi(D::max()), sum(D::zero()), n(0) {}

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