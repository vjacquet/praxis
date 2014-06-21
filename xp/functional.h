#ifndef __FUNCTIONAL_H__
#define __FUNCTIONAL_H__

#include <functional>

#include "fakeconcepts.h"

namespace xp {

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

	//memoize


} // namespace xp

#endif __FUNCTIONAL_H__