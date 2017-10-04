#include <iostream>

#include "../flags.h"

#include "../tests/testbench.h"

namespace user {
	enum struct my_bitmask : short { zero = 0, first = 1, second = 2, third = 4 };

	using namespace xp::flag_ops;
}

template<>
struct xp::flags::is_flags<user::my_bitmask> : std::true_type
{
};

TESTBENCH()

TEST(can_bitwise_or) {
	using namespace user;

	auto a = my_bitmask::first;
	auto b = my_bitmask::second;

	auto c = a | b;
	VERIFY_EQ(3, static_cast<int>(c));
}

TEST(can_bitwise_and) {
	using namespace user;

	auto a = my_bitmask::first;
	auto b = my_bitmask::second;

	auto c = a & b;
	VERIFY_EQ(0, static_cast<int>(c));
}

TEST(can_bitwise_xor) {
	using namespace user;

	auto a = my_bitmask::first;
	auto b = my_bitmask::second;

	auto c = a ^ b;
	VERIFY_EQ(3, static_cast<int>(c));
}

TEST(check_underlying_type) {
	using namespace user;

	auto p = typeid(std::underlying_type<user::my_bitmask>::type).name();
	std::cout << "  type: " << p << '\n';
}

TEST(can_negate) {
	using namespace user;

	auto a = my_bitmask::first;
	auto b = my_bitmask::second;

	auto c = a & ~b;
	VERIFY_EQ(1, static_cast<int>(c));
}

TESTFIXTURE(flags)