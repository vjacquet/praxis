#include <iostream>

#include "../flags.h"

#include "../tests/testbench.h"

namespace user {
enum struct my_bitmask : short { zero = 0, first = 1, second = 2, third = 4 };
	constexpr bool enable_flags_operators(my_bitmask) { return true; }

	//using xp::flags::operator |;
	using namespace xp::flags;
}

TESTBENCH()

TEST(can_bitwise_or) {
	using namespace xp::flag_ops;

	auto a{ user::my_bitmask::first };
	auto b{ user::my_bitmask::second };

	auto c = a | b;
	VERIFY_EQ(3, static_cast<int>(c));
}

TEST(can_bitwise_and) {
	using namespace xp::flag_ops;

	auto a{ user::my_bitmask::first };
	auto b{ user::my_bitmask::second };

	auto c = a & b;
	VERIFY_EQ(0, static_cast<int>(c));
}

TEST(can_bitwise_xor) {
	using namespace xp::flag_ops;

	auto a{ user::my_bitmask::first };
	auto b{ user::my_bitmask::second };

	auto c = a ^ b;
	VERIFY_EQ(3, static_cast<int>(c));
}

TEST(check_underlyingt_type) {
	using namespace xp::flag_ops;

	auto a{ user::my_bitmask::zero };

	auto p = typeid(std::underlying_type<user::my_bitmask>::type).name();
	std::cout << "  type: " << p << '\n';
}

TEST(can_negate) {
	using namespace xp::flag_ops;

	auto a{ user::my_bitmask::first };
	auto b{ user::my_bitmask::second };

	auto c = a & ~b;
	VERIFY_EQ(1, static_cast<int>(c));
}

TESTFIXTURE(flags)