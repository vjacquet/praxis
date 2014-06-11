#ifndef __INSTRUMENTED_H__
#define __INSTRUMENTED_H__

#include "fakeconcepts.h"

namespace xp {

	struct instrumented_base {
		enum operations { default_construct, construct, copy_construct, move_construct, copy_assign, move_assign, equal, compare, destruct, n };
		static int counts[n];
		static const char* names[n];

		static void reset();
	};


	template<Semiregular T>
	struct instrumented : instrumented_base {
		typedef T value_type;

		T value;

		// conversion
		instrumented(const T& v) : value(v) {
			++counts[construct];
		}

		// Semiregular
		instrumented() : value() {
			++counts[default_construct];
		}

		instrumented(const instrumented& x) : value(x.value) {
			++counts[copy_construct];
		}
		instrumented( instrumented&& x) : value(std::move(x.value)) {
			++counts[move_construct];
		}

		~instrumented() {
			++counts[destruct];
		}

		instrumented& operator=(const instrumented& x) {
			++counts[copy_assign];
			value = x.value;
			return *this;
		}
		instrumented& operator=(instrumented&& x) {
			++counts[move_assign];
			value = std::move(x.value);
			return *this;
		}

		// Regular
		inline friend bool operator==(const instrumented& x, const instrumented& y) {
			++counts[equal];
			return x.value == y.value;
		}
		inline friend bool operator!=(const instrumented& x, const instrumented& y) {
			return !(x == y);
		}

		// TotallyOrdered
		inline friend bool operator<(const instrumented& x, const instrumented& y) {
			++counts[compare];
			return x.value < y.value;
		}
		inline friend bool operator <=(const instrumented& x, const instrumented& y) {
			return !(y < x);
		}
		inline friend bool operator >(const instrumented& x, const instrumented& y) {
			return y < x;
		}
		inline friend bool operator >=(const instrumented& x, const instrumented& y) {
			return !(x < y);
		}
	};

	template<typename T>
	inline instrumented<T> instrument(T&& val) {
		return instrumented<T>(std::forward<T>(val));
	}

}

#endif __INSTRUMENTED_H__