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
		typedef typename integral_constant<bool, is_default_constructible<R>::value
			|| is_trivially_default_constructible<R>::value
			|| is_nothrow_default_constructible<R>::value>::type is_result_default_constructible_type;
		return details::memoize(std::move(func), is_result_default_constructible_type());
	}

	template<Function Op>
	class dereference_function_t : public details::functor_base<Op, function_traits<Op>::arity> {
		Op op;

	public:
		dereference_function_t(Op op) : op(op) {}

		template<Pointer P, class = std::enable_if<function_traits<Op>::arity==2>::type>
		auto operator()(const P& x, const P& y) -> decltype(op(*x, *y)) {
			return op(*x, *y);
		}
		template<Pointer P, class = std::enable_if<function_traits<Op>::arity == 2>::type>
		auto operator()(const P& x, const P& y) const -> decltype(op(*x, *y)) {
			return op(*x, *y);
		}

		template<Pointer P, class = std::enable_if<function_traits<Op>::arity == 1>::type>
		auto operator()(const P& x) -> decltype(op(*x)) {
			return op(*x);
		}
		template<Pointer P, class = std::enable_if<function_traits<Op>::arity == 1>::type>
		auto operator()(const P& x) const -> decltype(op(*x)) {
			return op(*x);
		}
	};

	template<Function Op>
	dereference_function_t<Op> dereference(Op op) {
		return dereference_function_t<Op> {op};
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

} // namespace xp

#endif __FUNCTIONAL_H__