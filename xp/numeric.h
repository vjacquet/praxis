#ifndef __NUMERIC_H__
#define __NUMERIC_H__

#include <algorithm>
#include <chrono>
#include <random>
#include "fakeconcepts.h"

// work in progress.
// check Knuth's AoCP Vol 2, p 266 for operations on non negative integers.

namespace xp {

	namespace integers {

		template<Integer N>
		N successor(const N& n) {
			return n + N(1);
		}

		template<Integer N>
		N predecessor(const N& n) {
			return n - N(1);
		}

		template<Integer N>
		N twice(const N& n) {
			return n + n;
		}

		template<Integer N>
		N half_nonnegative(const N& n) {
			return n >> N(1);
		}

		template<Integer N>
		N binary_scale_down_nonnegative(const N& n, const N& k) {
			return n >> k;
		}

		template<Integer N>
		N binary_scale_up_nonnegative(const N& n, const N& k) {
			return n << k;
		}

		template<Integer N>
		bool positive(const N& n) {
			return N(0) < n;
		}

		template<Integer N>
		bool negative(const N& n) {
			return n < N(0);
		}

		template<Integer N>
		bool zero(const N& n) {
			return N(0) == n;
		}

		template<Integer N>
		bool one(const N& n) {
			return N(1) == n;
		}

		template<Integer N>
		bool even(const N& n) {
			return (n & N(1)) == N(0);
		}

		template<Integer N>
		bool odd(const N& n) {
			return (n & N(1)) != N(0);
		}
	}

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

	template<ForwardIterator I, typename T>
	I reverse_iota(I first, I last, T val) {
		for (; first != last; ++first, --val)
			*first = val;
		return first;
	}

	template<OutputIterator O, typename N, typename T>
	O reverse_iota_n(O first, N n, T val)
	{
		for (; 0 < n; --n, ++first, --val)
			*first = val;
		return first;
	}

	template<RandomAccessIterator I, typename T, class URNG>
	I random_iota(I first, I last, T val, URNG&& g) {
		std::iota(first, last, val);
		std::shuffle(first, last, std::forward<URNG>(g));
		return last;
	}

	template<RandomAccessIterator I, typename T>
	I random_iota(I first, I last, T val) {
		std::random_device seed;
		return random_iota(first, last, val, std::default_random_engine {seed()});
	}

	template<RandomAccessIterator I, typename N, typename T, class URNG>
	I random_iota_n(I first, N n, T val, URNG&& g) {
		auto last = iota_n(first, n, val);
		std::shuffle(first, last, std::forward<URNG>(g));
		return last;
	}

	template<RandomAccessIterator I, typename N, typename T>
	I random_iota_n(I first, N n, T val) {
		std::random_device seed;
		return random_iota_n(first, n, val, std::default_random_engine {seed()});
	}

	template<InputIterator I, Integer N, typename T, BinaryOperation Op>
	T accumulate_n(I first, N n, T init, Op op) {
		while (n != 0) {
			init = op(init, *first);
			--n;
			++first;
		}
		return init;
	}
	template<InputIterator I, Integer N, typename T>
	T accumulate_n(I first, N n, T init) {
		return accumulate_n(first, n, init, std::plus<T>());
	}

	template<InputIterator I, Integer N, BinaryOperation Op>
	auto accumulate_n_nonempty(I first, N n, Op op)->typename std::decay<decltype(*first)>::type {
		// precondition: n > 0
		auto val = *first;
		while (--n != 0) {
			val = op(val, *++first);
		}
		return val;
	}
	template<InputIterator I, Integer N>
	auto accumulate_n_nonempty(I first, N n) -> typename std::decay<decltype(*first)>::type {
		// precondition: n > 0
		typedef typename std::decay<decltype(*first)>::type T;
		return accumulate_n_nonempty(first, n, std::plus<T>());
	}

	template<InputIterator I1, InputIterator I2, Integer N, typename T, BinaryOperation Op1, BinaryOperation Op2>
	T inner_product_n(I1 first1, I2 first2, N n, T init, Op1 op1, Op2 op2) {
		while (n != 0) {
			init = op1(init, op2(*first1, *first2));
			--n;
			++first1;
			++first2;
		}
		return init;
	}
	template<InputIterator I1, InputIterator I2, Integer N, typename T>
	T inner_product_n(I1 first1, I2 first2, N n, T init) {
		return inner_product_n(first1, first2, n, init, std::plus<T>(), std::multiplies<T>());
	}

	template<InputIterator I1, InputIterator I2, Integer N, BinaryOperation Op1, BinaryOperation Op2>
	auto inner_product_n_nonempty(I1 first1, I2 first2, N n, Op1 op1, Op2 op2)->decltype(op2(*first1, *first2)){
		// precondition: n > 0
		auto val = op2(*first1, *first2);
		while (--n != 0) {
			val = op1(val, op2(*++first1, *++first2));
		}
		return val;
	}
	template<InputIterator I1, InputIterator I2, Integer N>
	auto inner_product_n_nonempty(I1 first1, I2 first2, N n)->decltype(std::multiplies<>()(*first1, *first2)){
		// precondition: n > 0
		typedef std::decay<decltype(*first1)>::type T;
		return inner_product_n_nonempty(first1, first2, n, std::plus<T>(), std::multiplies<T>());
	}

	template<InputIterator I, Integer N, OutputIterator O, BinaryOperation Op>
	O adjacent_difference_n_nonempty(I first, N n, O result, Op op) {
		// precondition: n > 0
		auto prev = *first;
		*result = prev;
		decltype(prev) val;
		while (--n != 0) {
			val = *++first;
			*++result = op(val, prev);
			prev = val;
		}
		++result;
		return result;
	}
	template<InputIterator I, Integer N, OutputIterator O>
	O adjacent_difference_n_nonempty(I first, N n, O result) {
		// precondition: n > 0
		typedef std::decay<decltype(*first)>::type T;
		return adjacent_difference_n_nonempty(first, n, std::minus<T>());
	}

	template<InputIterator I, Integer N, OutputIterator O, BinaryOperation Op>
	O adjacent_difference_n(I first, N n, O result, Op op) {
		if (n == 0)
			return result;
		return adjacent_difference_n_nonempty(first, n, result, op);
	}
	template<InputIterator I, Integer N, OutputIterator O>
	O adjacent_difference_n(I first, N n, O result) {
		typedef std::decay<decltype(*first)>::type T;
		return adjacent_difference_n(first, n, result, std::minus<T>());
	}

	template<InputIterator I, Integer N, OutputIterator O, BinaryOperation Op>
	O partial_sum_n_nonempty(I first, N n, O result, Op op) {
		// precondition: n > 0
		auto val = *first;
		*result = val;
		while (--n != 0) {
			val = op(val, *++first);
			*++result = val;
		}
		++result;
		return result;
	}
	template<InputIterator I, Integer N, OutputIterator O>
	O partial_sum_n_nonempty(I first, N n, O result) {
		// precondition: n > 0
		typedef std::decay<decltype(*first)>::type T;
		return partial_sum_n_nonempty(first, n, result, std::plus<T>());
	}

	template<InputIterator I, Integer N, OutputIterator O, BinaryOperation Op>
	O partial_sum_n(I first, N n, O result, Op op) {
		if (n == 0) 
			return result;
		return partial_sum_n(first, n, result, op);
	}
	template<InputIterator I, Integer N, OutputIterator O>
	O partial_sum_n(I first, N n, O result) {
		typedef std::decay<decltype(*first)>::type T;
		return partial_sum_n(first, n, result, std:plus<T>());
	}

} // namespace xp

#endif __NUMERIC_H__