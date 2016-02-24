#ifndef __EXPR_H__
#define __EXPR_H__

#include <functional>
#include "../fakeconcepts.h"

namespace xp {

	//template<UnaryOperation Op, typename T>
	//struct expr {
	//	T const& t;
	//};

	//template<UnaryOperation Op, typename T>
	//auto eval(expr<Op, T> const& e) {
	//	Op op;
	//	return op(eval(e.t));
	//}

	template<BinaryOperation Op, typename L, typename R>
	struct expr {
		L const& l;
		R const& r;
	};

	template<BinaryOperation Op, typename L, typename R>
	auto eval(expr<Op, L, R> const& e) {
		Op op;
		return op(eval(e.l), eval(e.r));
	}

	template<typename T>
	auto eval(T const& x) {
		return x;
	}

	template<typename L, typename R>
	auto operator +(L const& l, R const& r)-> expr<std::plus<void>, L, R> {
		return{ l,r };
	}

	template<typename L, typename R>
	auto operator -(L const& l, R const& r)-> expr<std::minus<void>, L, R> {
		return{ l,r };
	}

	template<typename L, typename R>
	auto operator *(L const& l, R const& r)-> expr<std::multiplies<void>, L, R> {
		return{ l,r };
	}

	template<typename L, typename R>
	auto operator /(L const& l, R const& r)-> expr<std::divides<void>, L, R> {
		return{ l,r };
	}

	//template<typename T>
	//auto operator -(T const& t)-> expr<std::negate<void>, T> {
	//	return{ t };
	//}

}

#endif __EXPR_H__