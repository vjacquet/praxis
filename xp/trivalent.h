#ifndef __TRIVALENT_H__
#define __TRIVALENT_H__

#include <cstdint>

namespace xp {

	class trivalent
	{
		std::int8_t v;
		trivalent(std::int8_t v) : v(v) { }

	public:
		trivalent() : v(0) { }
		trivalent(bool b) : v(b ? 1 : -1) {}
		trivalent(const trivalent& other) : v(other.v) {}

		trivalent& operator =(trivalent other) {
			v = other.v;
		}
		trivalent operator !() { return trivalent((std::int8_t)-v); }

		friend bool is_true(trivalent tv) { return tv.v == 1; }
		friend bool is_false(trivalent tv) { return tv.v == -1; }
		friend bool is_unknown(trivalent tv) { return tv.v == 0; }

		friend inline trivalent operator ==(trivalent lhs, trivalent rhs) {
			if (is_unknown(rhs) || is_unknown(rhs)) return trivalent();
			return is_true(lhs) == is_true(rhs);
		}
		friend inline trivalent operator !=(trivalent lhs, trivalent rhs) {
			return !(lhs == rhs);
		}

		friend inline trivalent operator && (trivalent lhs, trivalent rhs) {
			if (is_unknown(lhs) || is_unknown(rhs)) return trivalent();
			return is_true(lhs) && is_true(rhs);
		}
		friend inline trivalent operator ||(trivalent lhs, trivalent rhs) {
			if (is_true(lhs) || is_true(rhs)) return true;
			if (is_false(lhs) && is_false(rhs)) return false;
			return trivalent();
		}
	};

	const trivalent unknown;

}


#endif __TRIVALENT_H__
