#ifndef __FLAGS_H__
#define __FLAGS_H__

// based on Jon Kalb, http://www.slashslash.info/slides/EnumBitfields.pdf
// Which is itslef based on a article in Overload magazine by Anothy Williams

#include <type_traits>

#include "fakeconcepts.h"

namespace xp {

namespace flags {

template<typename E>
constexpr bool enable_flags_operators(E) { return false; }

template<typename E>
constexpr
typename std::enable_if<enable_flags_operators(E{}), E>::type&
operator |=(E& x, E y) {
	using underlying = typename std::underlying_type<E>::type;

	return x = static_cast<E>(static_cast<underlying>(x) | static_cast<underlying>(y));
}

template<typename E>
constexpr 
typename std::enable_if<enable_flags_operators(E{}), E>::type
operator |(E x, E y) {
	return x |= y;
}

template<typename E>
constexpr
typename std::enable_if<enable_flags_operators(E{}), E>::type&
operator &=(E& x, E y) {
	using underlying = typename std::underlying_type<E>::type;

	return x = static_cast<E>(static_cast<underlying>(x) & static_cast<underlying>(y));
}

template<typename E>
constexpr
typename std::enable_if<enable_flags_operators(E{}), E>::type
operator &(E x, E y) {
	return x &= y;
}

template<typename E>
constexpr
typename std::enable_if<enable_flags_operators(E{}), E>::type&
operator ^=(E& x, E y) {
	using underlying = typename std::underlying_type<E>::type;

	return x = static_cast<E>(static_cast<underlying>(x) ^ static_cast<underlying>(y));
}

template<typename E>
constexpr
typename std::enable_if<enable_flags_operators(E{}), E>::type
operator ^(E x, E y) {
	return x ^= y;
}

template<typename E>
constexpr
typename std::enable_if<enable_flags_operators(E{}), E>::type
operator ~(E x) {
	using underlying = typename std::underlying_type<E>::type;

	return static_cast<E>(~static_cast<underlying>(x));
}

}

namespace flag_ops {

using ::xp::flags::operator|;
using ::xp::flags::operator|=;
using ::xp::flags::operator&;
using ::xp::flags::operator&=;
using ::xp::flags::operator^;
using ::xp::flags::operator^=;
using ::xp::flags::operator~;

}

}

#endif __FLAGS_H__
