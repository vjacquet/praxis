#ifndef __EXPR_H__
#define __EXPR_H__

#include <functional>
#include "../fakeconcepts.h"

namespace xp {

	template<BinaryOperation Op, typename L, typename R>
	struct expr {
		L const& l;
		R const& r;
	};

	template<BinaryOperation Op, typename L, typename R>
	auto eval(expr<Op, L, R> const& e) {
		Op op;
		return op(e.l, e.r);
	}

	template<typename T>
	auto eval(T const& x) {
		return x;
	}

	template<typename L, typename R>
	auto operator *(L const& l, R const& r)-> expr<std::multiplies<void>, L, R> {
		return{ l,r };
	}
}

#endif __EXPR_H__