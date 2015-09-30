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
		instrumented(T const& v) : value(v) {
			++counts[construct];
		}

		// Semiregular
		instrumented() : value() {
			++counts[default_construct];
		}

		instrumented(instrumented const& x) : value(x.value) {
			++counts[copy_construct];
		}
		instrumented(instrumented&& x) noexcept : value(std::move_if_noexcept(x.value)) {
			++counts[move_construct];
		}

		~instrumented() {
			++counts[destruct];
		}

		instrumented& operator=(instrumented const& x) {
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
		inline friend bool operator==(instrumented const& x, instrumented const& y) {
			++counts[equal];
			return x.value == y.value;
		}
		inline friend bool operator!=(instrumented const& x, instrumented const& y) {
			return !(x == y);
		}

		// TotallyOrdered
		inline friend bool operator<(instrumented const& x, instrumented const& y) {
			++counts[compare];
			return x.value < y.value;
		}
		inline friend bool operator <=(instrumented const& x, instrumented const& y) {
			return !(y < x);
		}
		inline friend bool operator >(instrumented const& x, instrumented const& y) {
			return y < x;
		}
		inline friend bool operator >=(instrumented const& x, instrumented const& y) {
			return !(x < y);
		}
	};

	template<typename T>
	inline instrumented<T> instrument(T&& val) {
		return instrumented<T>(std::forward<T>(val));
	}

}

#endif __INSTRUMENTED_H__