#ifndef __FLAGS_H__
#define __FLAGS_H__

// based on Jon Kalb, http://www.slashslash.info/slides/EnumBitfields.pdf
// Which is itslef based on a article in Overload magazine by Anothy Williams

#include <type_traits>

#include "fakeconcepts.h"

namespace xp {

namespace flags {

template<class E>
struct is_flags : std::false_type
{
};

template<class T>
constexpr bool is_flags_v = is_flags<T>::value;

template<typename E>
constexpr
typename std::enable_if<is_flags_v<E>, E>::type&
operator |=(E& x, E y) {
	static_assert(std::is_enum_v<E>, "The flag must be an enum.");

	using underlying = typename std::underlying_type<E>::type;

	return x = static_cast<E>(static_cast<underlying>(x) | static_cast<underlying>(y));
}

template<typename E>
constexpr 
typename std::enable_if<is_flags_v<E>, E>::type
operator |(E x, E y) {
	return x |= y;
}

template<typename E>
constexpr
typename std::enable_if<is_flags_v<E>, E>::type&
operator &=(E& x, E y) {
	static_assert(std::is_enum_v<E>, "The flag must be an enum.");

	using underlying = typename std::underlying_type<E>::type;

	return x = static_cast<E>(static_cast<underlying>(x) & static_cast<underlying>(y));
}

template<typename E>
constexpr
typename std::enable_if<is_flags_v<E>, E>::type
operator &(E x, E y) {
	return x &= y;
}

template<typename E>
constexpr
typename std::enable_if<is_flags_v<E>, E>::type&
operator ^=(E& x, E y) {
	static_assert(std::is_enum_v<E>, "The flag must be an enum.");

	using underlying = typename std::underlying_type<E>::type;

	return x = static_cast<E>(static_cast<underlying>(x) ^ static_cast<underlying>(y));
}

template<typename E>
constexpr
typename std::enable_if<is_flags_v<E>, E>::type
operator ^(E x, E y) {
	return x ^= y;
}

template<typename E>
constexpr
typename std::enable_if<is_flags_v<E>, E>::type
operator ~(E x) {
	static_assert(std::is_enum_v<E>, "The flag must be an enum.");

	using underlying = typename std::underlying_type<E>::type;

	return static_cast<E>(~static_cast<underlying>(x));
}

}

namespace flag_ops {

using xp::flags::operator|;
using xp::flags::operator|=;
using xp::flags::operator&;
using xp::flags::operator&=;
using xp::flags::operator^;
using xp::flags::operator^=;
using xp::flags::operator~;

}

}

#endif //__FLAGS_H__
