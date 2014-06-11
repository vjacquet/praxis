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

#include <exception>
#include <iostream>
#include <sstream>
#include <string>

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

#define TESTFIXTURE(b)  /* unnamed namespace { */ } \
	void b(size_t& pass, size_t& fail) { testrange<0, __LINE__>().run(pass, fail); }
