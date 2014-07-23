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
	class timer {
	private:
		typedef typename C::time_point time_point;
		time_point since;

	public:
		typedef typename C::duration duration;

		timer() : since(C::now()) {}
		timer(const timer& x) : since(x.since) {}

		void reset() {
			since = C::now();
		}

		template<typename D>
		requires(D is Duration)
		D elapsed() const {
			return std::chrono::duration_cast<D>(C::now() - since);
		}

		// Regular
		inline friend bool operator==(const timer& x, const timer& y) {
			return x.since == y.since;
		}
		inline friend bool operator!=(const timer& x, const timer& y) {
			return !(x == y);
		}

		// TotallyOrdered
		inline friend bool operator<(const timer& x, const timer& y) {
			// we want to compare on duration, not epoch.
			return y.since < x.since;
		}
		inline friend bool operator <=(const timer& x, const timer& y) {
			return !(y < x);
		}
		inline friend bool operator >(const timer& x, const timer& y) {
			return y < x;
		}
		inline friend bool operator >=(const timer& x, const timer& y) {
			return !(x < y);
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
	class measures {
		D   mini;
		D   maxi;
		D   sum;
		int n;

	public:
		measures() : mini(D::max()), maxi(D::min()), sum(D::zero()), n(0) {}

		measures& operator += (D d) {
			if (d < mini) mini = d;
			if (maxi < d) maxi = d;
			sum += d;
			n++;
			return *this;
		}
		D min() const { assert(n > 0); return mini; }
		D max() const { assert(n > 0); return maxi; }
		D avg() const { assert(n > 0); return sum / n; }
	};

} // namespace xp

#endif __BENCHMARK_H__