#ifndef __TRIVALENT_H__
#define __TRIVALENT_H__

#include <cstdint>

namespace xp {

	class trivalent
	{
		std::int8_t v;
		trivalent(int v) : v((std::int8_t)v) { }

	public:
		trivalent() : v(0) { }
		trivalent(bool b) : v(b ? 1 : -1) {}
		trivalent(const trivalent& other) : v(other.v) {}

		trivalent& operator =(trivalent other) {
			v = other.v;
			return *this;
		}
		trivalent operator !() { return trivalent(-v); }

		friend bool is_true(trivalent tv) { return tv.v == 1; }
		friend bool is_false(trivalent tv) { return tv.v == -1; }
		friend bool is_unknown(trivalent tv) { return tv.v == 0; }

		// semiregular. returning trivalent makes comparison too difficult.
		// checking unknown is easy because unknown != unknown
		inline friend bool operator ==(trivalent lhs, trivalent rhs) {
			return (lhs.v*rhs.v) > 0;
		}
		inline friend bool operator !=(trivalent lhs, trivalent rhs) {
			return !(lhs == rhs);
		}

		// regular: unknown < false < true
		inline friend bool operator<(trivalent lhs, trivalent rhs) {
			if (is_unknown(lhs))
				return !is_unknown(rhs);
			if (is_unknown(rhs))
				return false;
			return lhs.v < rhs.v;
		}
		inline friend bool operator <=(const trivalent& x, const trivalent& y) {
			return !(y < x);
		}
		inline friend bool operator >(const trivalent& x, const trivalent& y) {
			return y < x;
		}
		inline friend bool operator >=(const trivalent& x, const trivalent& y) {
			return !(x < y);
		}

		// logical operators
		inline friend trivalent operator &&(trivalent lhs, trivalent rhs) {
			return trivalent(std::min(lhs.v, rhs.v));
		}
		inline friend trivalent operator ||(trivalent lhs, trivalent rhs) {
			return trivalent(std::max(lhs.v, rhs.v));
		}

		inline friend bool equivalent(trivalent lhs, trivalent rhs) {
			// unknown != unknown on purpose so I need this function to check for equivalence
			// but is it a good idea. Should I make unkown == unkown ?
			// anyway, this function could be included in "functional.h", defined as
			// !((x < y) || (y < x)) or !(pred(x,y) || pred(y, x))
			return lhs.v == rhs.v;
		}
	};

	const trivalent unknown;

}

#endif __TRIVALENT_H__
