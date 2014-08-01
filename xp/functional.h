#ifndef __FUNCTIONAL_H__
#define __FUNCTIONAL_H__

#include <functional>
#include <map>
#include <tuple>
#include <type_traits>
#include <utility>

#include "fakeconcepts.h"

namespace xp {
	namespace details {

		template<Function F, int Arity>
		struct functor_base {
			typedef typename function_traits<F>::return_type result_type;
		};

		template<Function F>
		struct functor_base<F, 1> {
			typedef typename function_traits<F>::return_type result_type;
			typedef typename function_traits<F>::argument<0>::type argument_type;
		};

		template<Function F>
		struct functor_base<F, 2> {
			typedef typename function_traits<F>::return_type result_type;
			typedef typename function_traits<F>::argument<0>::type first_argument_type;
			typedef typename function_traits<F>::argument<1>::type second_argument_type;
		};

		template <typename R, typename... Args>
		std::function<R(Args...)> memoize(std::function<R(Args...)>&& func, std::true_type) {
			// when R is default constructible
			using namespace std;

			map<tuple<Args...>, R> cache;
			return ([=](Args... args) mutable {
				tuple<Args...> t(args...);
				bool inserted;
				map<tuple<Args...>, R>::iterator pos;
				tie(pos, inserted) = cache.insert(make_pair(t, R {}));
				if (inserted) {
					pos->second = func(args...);
				}
				return pos->second;
			});
		}

		template <typename R, typename... Args>
		std::function<R(Args...)> memoize(std::function<R(Args...)>&& func, std::false_type) {
			// when R is not default constructible
			using namespace std;

			map<tuple<Args...>, R> cache;
			return ([=](Args... args) mutable {
				tuple<Args...> t(args...);
				auto found = cache.find(t);
				if (found == cache.end()) {
					tie(found, std::ignore) = cache.emplace(t, func(args...));
				}
				return found->second;
			});
		}
	}

	//memoize
	template <typename R, typename... Args>
	std::function<R(Args...)> memoize(std::function<R(Args...)> func) {
		using namespace std;
		typedef typename integral_constant < bool, is_default_constructible<R>::value
			|| is_trivially_default_constructible<R>::value
			|| is_nothrow_default_constructible<R>::value > ::type is_result_default_constructible_type;
		return details::memoize(std::move(func), is_result_default_constructible_type());
	}

	template<Function Op>
	class dereference_function_t : public details::functor_base < Op, function_traits<Op>::arity > {
		Op op;

	public:
		dereference_function_t(Op op) : op(op) {}

		template<typename P, class = std::enable_if<function_traits<Op>::arity == 2>::type>
		auto operator()(const P& x, const P& y) -> decltype(op(*x, *y)) {
			return op(*x, *y);
		}
		template<typename P, class = std::enable_if<function_traits<Op>::arity == 2>::type>
		auto operator()(const P& x, const P& y) const -> decltype(op(*x, *y)) {
			return op(*x, *y);
		}

		template<typename P, class = std::enable_if<function_traits<Op>::arity == 1>::type>
		auto operator()(const P& x) -> decltype(op(*x)) {
			return op(*x);
		}
		template<typename P, class = std::enable_if<function_traits<Op>::arity == 1>::type>
		auto operator()(const P& x) const -> decltype(op(*x)) {
			return op(*x);
		}
	};

	template<Function Op>
	dereference_function_t<Op> dereference(Op op) {
		return dereference_function_t < Op > {op};
	}

	template<Function Op>
	class negation_function_t : public details::functor_base < Op, function_traits<Op>::arity > {
		Op op;

	public:
		negation_function_t(Op op) : op(op) {}

		template<typename T, class = std::enable_if<function_traits<Op>::arity == 2>::type>
		auto operator()(const T& x, const T& y) -> decltype(op(x, y)) {
			return !op(x, y);
		}
		template<typename T, class = std::enable_if<function_traits<Op>::arity == 2>::type>
		auto operator()(typename const T& x, const T& y) const -> decltype(op(x, y)) {
			return !op(x, y);
		}

		template<typename T, class = std::enable_if<function_traits<Op>::arity == 1>::type>
		auto operator()(const T& x) -> decltype(op(x)) {
			return !op(x);
		}
		template<typename T, class = std::enable_if<function_traits<Op>::arity == 1>::type>
		auto operator()(const T& x) const -> decltype(op(x)) {
			return !op(x);
		}
	};

	template<Function Op>
	negation_function_t<Op> negation(Op op) {
		return negation_function_t<Op>(op);
	}

	template<BinaryOperation Op>
	class transpose_function_t {
		Op op;
		typedef Domain(Op) T;
	public:
		transpose_function_t(Op op) : op(op) {}

		T operator()(const T& x, const T& y) {
			return op(y, x);
		}
	};

	template<BinaryOperation Op>
	inline transpose_function_t<Op> transpose(Op op) {
		return transpose_function_t<Op>(op);
	}

	template<StrictWeakOrdering Compare>
	class between_t {
		typedef typename Compare::first_argument_type argument_type;
		argument_type lo;
		argument_type up;
		Compare cmp;

	public:
		between_t(argument_type&& lower, argument_type&& upper, Compare&& cmp)
			: lo(std::forward<argument_type>(lower))
			, up(std::forward<argument_type>(upper))
			, cmp(cmp) {}

		bool operator()(const argument_type& x) const {
			return !(cmp(x, lo) || cmp(up, x));
		}
	};

	template <typename T>
	between_t<std::less<T>> between(T&& lower, T&& upper) {
		// precondition: lower <= upper
		return between_t<std::less<T>>(std::forward<T>(lower), std::forward<T>(upper), std::less<T> {});
	}

	template <typename T, StrictWeakOrdering Compare>
	between_t<Compare> between(const Compare& cmp, T&& lower, T&& upper) {
		// precondition: !cmp(upper, lower)
		return between_t<Compare>(std::forward(lower), std::forward(upper), cmp);
	}

	// from sgi extensions
	template<typename T>
	struct identity {
		typedef T argument_type;
		typedef T result_type;

		const T& operator()(const T& x) const {
			return x;
		}
	};

	// adapted from sgi extensions select1st and select2nd
	template<size_t I, typename T>
	struct select_element {
		typedef T argument_type;
		typedef typename std::tuple_element<I, T>::type result_type;

		const result_type& operator()(const argument_type& x) const {
			return std::get<I>(x);
		}
		result_type& operator()(argument_type& x) const {
			return std::get<I>(x);
		}
	};


	// synonyms
	using std::plus;
	using std::negate;
	using std::multiplies;

	template<typename T>
	T identity_element(plus<T>) {
		return T(0);
	}

	template<typename T>
	T identity_element(multiplies<T>) {
		return T(1);
	}

	template<typename T>
	struct reciprocal {
		// do not inherit from public std::unary_function<T, T> as it is deprecated (but why?)
		typedef T argument_type;
		typedef T result_type;

		T operator()(const T& x) {
			return identity_element(multiplies<T>()) / x;
		}
	};

	template<typename T>
	negate<T> inverse_operation(plus<T>) {
		return negate<T>();
	}

	template<typename T>
	reciprocal<T> inverse_operation(multiplies<T>) {
		return reciprocal<T>();
	}

	// from EoP
	template<typename Pred>
	struct complement_of_converse {
		Pred pred;
		typedef typename Pred::argument_type argument_type;

		complement_of_converse(Pred pred) :pred(pred) {}
		bool operator ()(const argument_type& x, const argument_type& y) {
			return !pred(b, a);
		}
	};

	// from EoP
	template<BinaryOperation Op>
	struct transpose_operation {
		Op op;

		typedef Codomain(Op) argument_type;

		argument_type operator() (const argument_type& x, const argument_type& y) {
			return op(y, x);
		}
	};

	namespace details {

		template<Regular T, Integer N, BinaryOperation Op>
		T power_accumulate_semigroup(T r, T a, N n, Op op) {
			using namespace xp::integers;

			// precondition: n >= 0
			if (n == 0) return r;
			while (true) {
				if (odd(n)) {
					r = op(r, a);
					if (n == 1) return r;
				}
				a = op(a, a);
				n = half_nonnegative(n);
			}
		}

	}

	template<Regular T, Integer N, BinaryOperation Op>
	T power_semigroup(T a, N n, Op op) {
		using namespace xp::integers;

		// precondition: n > 0
		while (!odd(n)) {
			a = op(a, a);
			n = half_nonnegative(n);
		}
		if (n == 1) return a;
		return details::power_accumulate_semigroup(a, op(a, a), half_nonnegative(n - 1), op);
	}
	template<Regular T, Integer N>
	T power_semigroup(T a, N n) {
		return power_monoid(a, n, multiplies<T>());
	}

	template<Regular T, Integer N, BinaryOperation Op>
	T power_monoid(T a, N n, Op op) {
		// precondition: n >= 0
		if (n == 0) return identity_element(op);
		return power_semigroup(a, n, op);
	}
	template<Regular T, Integer N>
	T power_monoid(T a, N n) {
		return power_monoid(a, n, multiplies<T>());
	}

	template<Regular T, Integer N, BinaryOperation Op>
	T power(T a, N n, Op op) {
		//static_assert(std::is_same<T, Domain<Op>>::value, "The domain of Op must be T.");
		if (n < 0) {
			n = -n;
			a = inverse_operation(op)(a);
		}
		return power_monoid(a, n, op);
	}
	template<Regular T, Integer N>
	T power(T a, N n) {
		return power(a, n, multiplies<T>());
	}

} // namespace xp

#endif __FUNCTIONAL_H__