// Initialize the test bench by calling TESTBENCH(),
// then define the tests and, finally,
// name the TESTFIXTURE.
// 
// To call the tests, write the following code in the main:
// size_t pass = 0, fail = 0;
// name_of_the_fixture(pass, fail);
// cout << "pass: " << pass << ", fail: " << fail << endl;
// if(fail) {
// 	 cerr << "ERRORS PRESENT." << endl;
// } else if(!pass) {
//   cerr << "ERROR, no tests run" << endl;
// }

#include <algorithm>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct fixture {
	const std::string name;

	virtual ~fixture() {}

	void operator()(size_t& pass, size_t& fail) const {
		std::cout << "FIXTURE: " << name << std::endl;
		run(pass, fail);
		std::cout << std::endl;
	}

protected:
	fixture(const char* name);

private:
	virtual void run(size_t& pass, size_t& fail) const = 0;
};

class testing_bench {
	friend struct fixture;
	std::vector<typename std::reference_wrapper<const fixture>> fixtures;

	static void run(const fixture& f, size_t& pass, size_t& fail) {
		f(pass, fail);
	}
	static testing_bench instance;

public:

	static void run(const char* fixture_name, size_t& pass, size_t& fail) {
		std::string name = fixture_name;
		auto first = instance.fixtures.cbegin();
		auto last = instance.fixtures.cend();
		auto found = std::find_if(first, last, [&name](const fixture& x) { return x.name == name; });
		if (found != last)
			run(*found, pass, fail);
	}

	static void run_all(size_t& pass, size_t& fail) {
		for (auto& f : instance.fixtures)
			run(f, pass, fail);
	}

};

inline fixture::fixture(const char* name) : name(name) {
	testing_bench::instance.fixtures.push_back(std::cref(*this));
}

#define TESTNAMESPACE namespace

#define TESTBENCH() TESTNAMESPACE {\
struct empty_testcase { void run() {} const char* name() { return 0; } };\
template <size_t offset> struct testcase : empty_testcase {}; \
template <size_t begin, size_t end> \
struct testrange {\
	void run(size_t& pass, size_t& fail) {\
		testcase<begin> a_case;\
		if(a_case.name()) {\
			size_t p = 0, f = 0;\
			std::cout << "TEST: Running " << a_case.name() << std::endl;\
			try {\
				a_case.run();\
				++pass; \
			}\
			catch(logic_error& e) {\
				std::cerr << "ERRORS:" << "  " << e.what() << std::endl;\
				++fail; \
			}\
		}\
		const size_t rem = (end-begin-1);\
		testrange<begin+1, begin+1+rem/2>().run(pass, fail);\
		testrange<begin+1+rem/2, end>().run(pass, fail);\
	}\
};\
template <size_t begin> struct testrange<begin, begin> { void run(size_t& pass, size_t& fail) {}; };

#define TEST(fun_name) void fun_name(); \
template <> \
struct testcase<__LINE__> { \
	const char* name() { return(#fun_name); } \
	void run() { fun_name(); } \
}; \
inline void fun_name()

#define Q_(e) #e
#define Q(e)  Q_(e)
#define TEST_WHERE  __FILE__ "(" Q(__LINE__) "): "
#define VERIFY(expr) \
{ auto e = (expr); if(!e) { throw std::logic_error(TEST_WHERE "VERIFY("#expr")"); }  }
#define VERIFY_EQ(expected, actual) \
{ auto e = (expected); auto a = (actual); \
if(!(e == a)) { \
	std::stringstream msg; \
	msg << TEST_WHERE "(" << e << ")!=(" << a << ") in VERIFY_EQ("#expected","#actual")"; \
	throw std::logic_error(msg.str()); \
}}
#define SKIP(expr) \
{ auto e = (expr); if(e) { cout << "SKIPPED" << endl; return; }  }

#define TESTFIXTURE(b) struct b##_fixture : public fixture { b##_fixture() : fixture(Q(b)) {} \
private: virtual void run(size_t& pass, size_t& fail) const { testrange<0, __LINE__>().run(pass, fail); } \
}; \
b##_fixture registered_##b##_fixture; \
/* unnamed namespace { */ }
