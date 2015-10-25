#include <string>
#include <type_traits>
#include <utility>

#include "../tests/testbench.h"

namespace herbsutter {

	class employee {
		std::string name_;
	public:
		std::string const& get_name() const {
			return name_;
		}

		template<class String, class = std::enable_if_t<std::is_same<std::decay_t<String>, std::string>::value>>
		void set_name(String&& name) /*noexcept(std::is_nothrow_assignable<std::string&, String>::value)*/ {
			name_ = std::forward<String>(name);
		}
	};

}

namespace my {

	class employee {
		std::string name_;
	public:
		const std::string& get_name() const {
			return name_;
		}

		template<class S>
		void set_name(S&& name) /*noexcept(std::is_nothrow_assignable<std::string&, String>::value)*/ {
			name_ = std::forward<S>(name);
		}
	};
}

TESTBENCH()

TEST(check_hs_employee) {
	using namespace herbsutter;

	std::string n = "test";

	employee e;
	e.set_name(n);
	VERIFY_EQ(n, e.get_name());
	//e.set_name("test");
	//VERIFY_EQ(n, e.get_name());
	e.set_name(std::string {"test"});
	VERIFY_EQ(n, e.get_name());
}

TEST(check_my_employee) {
	using namespace my;

	std::string n = "test";

	employee e;
	e.set_name(n);
	VERIFY_EQ(n, e.get_name());
	e.set_name("test");
	VERIFY_EQ(n, e.get_name());
	e.set_name(std::string {"test"});
	VERIFY_EQ(n, e.get_name());
}

TESTFIXTURE(perfect_forwarding)