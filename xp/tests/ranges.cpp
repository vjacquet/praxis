#include <string>

#include "../algorithm.h"
#include "testbench.h"

using namespace std;
using namespace xp;

TESTBENCH()

TEST(check_range_before_with_value) {
	string haystack {"aa-bb"};
	VERIFY(range_before(haystack, '-') == "aa");
	VERIFY(range_before(haystack, 'a') == "");
	VERIFY(range_before(haystack, 'b') == "aa-");
	VERIFY(range_before(haystack, 'q') == "");
}

TEST(check_range_after_with_value) {
	string haystack {"aa-bb"};
	VERIFY(range_after(haystack, '-') == "bb");
	VERIFY(range_after(haystack, 'a') == "a-bb");
	VERIFY(range_after(haystack, 'b') == "b");
	VERIFY(range_after(haystack, 'q') == "");
}

TEST(check_range_before_with_range) {
	string haystack {"aa-bb"};
	VERIFY(range_before(haystack, string {"-"}) == "aa");
	VERIFY(range_before(haystack, string {"a"}) == "");
	VERIFY(range_before(haystack, string {"b"}) == "aa-");
	VERIFY(range_before(haystack, string {"q"}) == "");
}

TEST(check_range_after_with_range) {
	string haystack {"aa-bb"};
	VERIFY(range_after(haystack, string {"-"}) == "bb");
	VERIFY(range_after(haystack, string {"a"}) == "a-bb");
	VERIFY(range_after(haystack, string {"b"}) == "b");
	VERIFY(range_after(haystack, string {"q"}) == "");
}

TEST(prove_range_after_with_c_string_fails) {
	string haystack {"aa-bb"};
	// Fails because std::cend includes the terminal '\0'
	VERIFY(range_after(haystack, "-") == "");
}

TEST(check_bounded_range) {
	string s {"hello word"};
	auto r = make_bounded_range(s.begin(), s.end());
	auto n = size(r);
	VERIFY(!empty(r));
	VERIFY(n == 10)
}

TEST(check_bounded_range_is_semiregular) {
	string s {"hello word"};

	bounded_range<string::iterator> dft;
	bounded_range<string::iterator> src(s.begin(), s.begin()+5);
	bounded_range<string::iterator> cpy = src;

	dft = src;

	VERIFY(src == cpy);
	VERIFY(src == dft);
}

TEST(check_counted_range) {
	string s {"hello word"};
	auto r = make_counted_range(s.begin(), s.find(' '));
	auto n = size(r);
	VERIFY(!empty(r));
	VERIFY(n == 5);
}

TEST(check_counted_range_is_semiregular) {
	string s {"hello word"};

	counted_range<string::iterator> dft;
	counted_range<string::iterator> src(s.begin(), 5);
	counted_range<string::iterator> cpy = src;

	dft = src;

	VERIFY(src == cpy);
	VERIFY(src == dft);
}

TESTFIXTURE(ranges)
