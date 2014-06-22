#ifndef __FUNCTIONAL_H__
#define __FUNCTIONAL_H__

#include <functional>
#include <map>
#include <tuple>
#include <utility>

#include "fakeconcepts.h"

namespace xp {
	//memoize
	template <typename ReturnType, typename... Args>
	std::function<ReturnType(Args...)> memoize(std::function<ReturnType(Args...)> func) {
		using namespace std;
		
		map<tuple<Args...>, ReturnType> cache;
		return ([=](Args... args) mutable {
			tuple<Args...> t(args...);
			bool inserted;
			map<tuple<Args...>, ReturnType>::iterator pos;
			tie(pos, inserted) = cache.insert(make_pair(t, ReturnType {}));
			if (inserted) {
				pos->second = func(args...);
			}
			return pos->second;
		});
	}

	template<Function Op>
	struct dereference_function_t {
		Op op;

		dereference_function_t(Op op) : op(op) {}

		template<Pointer P>
		auto operator()(const P& x, const P& y) -> decltype(op(*x, *y)) {
			return op(*x, *y);
		}
		template<Pointer P>
		auto operator()(const P& x, const P& y) const -> decltype(op(*x, *y)) {
			return op(*x, *y);
		}

		template<Pointer P>
		auto operator()(const P& x) -> decltype(op(*x)) {
			return op(*x);
		}
		template<Pointer P>
		auto operator()(const P& x) const -> decltype(op(*x)) {
			return op(*x);
		}
	};

	template<Function Op>
	dereference_function_t<Op> dereference(Op op) {
		return dereference_function_t<Op> {op};
	}

} // namespace xp

#endif __FUNCTIONAL_H__